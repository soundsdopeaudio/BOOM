// =============================================================
// FILE: Source/PluginEditor.cpp
// =============================================================
#include "PluginEditor.h"

BoomAudioProcessorEditor::BoomAudioProcessorEditor(BoomAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p), drumGrid(p), pianoRoll(p)
{
    setResizable(true, true); setSize(980, 640);

    // === Engine selectors ===
    auto engineChange = [this](boom::Engine e) { processor.apvts.getParameter(boom::ParamIDs::engine)->beginChangeGesture();
    dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter(boom::ParamIDs::engine))->operator=((int)e);
    processor.apvts.getParameter(boom::ParamIDs::engine)->endChangeGesture();
    syncEngineUI(); };

    engine808.onClick = [engineChange] { engineChange(boom::Engine::e808); };
    engineBass.onClick = [engineChange] { engineChange(boom::Engine::Bass); };
    engineDrums.onClick = [engineChange] { engineChange(boom::Engine::Drums); };

    // Engine buttons
    addAndMakeVisible(engine808);
    addAndMakeVisible(engineBass);
    addAndMakeVisible(engineDrums);

    // Image assets: 808Btn.png (+ _hover/_down), bassBtn.png, drumsBtn.png
    setButtonImages3(engine808, "808Btn_png", "808Btn_hover_png", "808Btn_down_png");
    setButtonImages3(engineBass, "bassBtn_png", "bassBtn_hover_png", "bassBtn_down_png");
    setButtonImages3(engineDrums, "drumsBtn_png", "drumsBtn_hover_png", "drumsBtn_down_png");


    addAndMakeVisible(generateBtn);
    addAndMakeVisible(dragMidiBtn);
    addAndMakeVisible(rollsBtn);
    addAndMakeVisible(aiBtn);
    addAndMakeVisible(bumpBtn);
    addAndMakeVisible(flipBtn);

    setButtonImages3(generateBtn, "generateBtn_png", "generateBtn_hover_png", "generateBtn_down_png");
    setButtonImages3(dragMidiBtn, "saveMidiBtn_png", "saveMidiBtn_hover_png", "saveMidiBtn_down_png"); // you named it saveMidiBtn; using that.
    setButtonImages3(rollsBtn, "rollsBtn_png", "rollsBtn_hover_png", "rollsBtn_down_png");
    setButtonImages3(aiBtn, "aiToolsBtn_png", "aiToolsBtn_hover_png", "aiToolsBtn_down_png");
    setButtonImages3(bumpBtn, "bumppitBtn_png", "bumppitBtn_hover_png", "bumppitBtn_down_png");
    setButtonImages3(flipBtn, "flippitBtn_png", "flippitBtn_hover_png", "flippitBtn_down_png");

    // Logo
    addAndMakeVisible(logoImg);
    logoImg.setImage(pngFromBinary("logo_png"));

    // Main window labels (use sparingly to avoid clutter)
    auto setLbl = [&](juce::ImageComponent& c, const char* name) { addAndMakeVisible(c); c.setImage(pngFromBinary(name)); };

    setLbl(engineLblImg, "engineLbl_png");
    setLbl(humanizeLblImg, "humanizeLbl_png");
    setLbl(keyLblImg, "keyLbl_png");
    setLbl(scaleLblImg, "scaleLbl_png");
    setLbl(timeSigLblImg, "timeSigLbl_png");
    setLbl(octaveLblImg, "octaveLbl_png");
    setLbl(styleLblImg, "styleLbl_png");
    setLbl(restDensityLblImg, "restDensityLbl_png");
    setLbl(barsLblImg, "barsLbl_png");

    setLbl(drumsLblImg, "drumsLbl_png");
    setLbl(drumsSelectorLblImg, "drumsSelectorLbl_png");
    setLbl(bassSelectorLblImg, "bassSelectorLbl_png");
    setLbl(_808BassLblImg, "808BassLbl_png");

    setLbl(aiToolsLblImg, "aiToolsLbl_png");
    setLbl(tripletsLblImg, "tripletsLbl_png");
    setLbl(dottedNotesLblImg, "dottedNotesLbl_png");

    addAndMakeVisible(tripletsToggle);
    addAndMakeVisible(dottedToggle);

    setButtonImages3(tripletsToggle, "toggleBtnOff_png", "toggleBtnOff_hover_png", "toggleBtnOff_down_png");
    setButtonImages3(dottedToggle, "toggleBtnOff_png", "toggleBtnOff_hover_png", "toggleBtnOff_down_png");

    // When state changes via APVTS, update images to On/Off accordingly
    auto updateToggleArt = [&](juce::ImageButton& b, bool isOn)
    {
        if (isOn)
            setButtonImages3(b, "toggleBtnOn_png", "toggleBtnOn_hover_png", "toggleBtnOn_down_png");
        else
            setButtonImages3(b, "toggleBtnOff_png", "toggleBtnOff_hover_png", "toggleBtnOff_down_png");
    };

    // Hook APVTS listeners to refresh artwork (optional quick way):
    tripletsToggle.onClick = [&, p = &processor.apvts] {
        auto* param = dynamic_cast<juce::AudioParameterBool*>(p->getParameter(boom::ParamIDs::useTriplets));
        param->operator=(!param->get()); // flip
        updateToggleArt(tripletsToggle, param->get());
    };

    dottedToggle.onClick = [&, p = &processor.apvts] {
        auto* param = dynamic_cast<juce::AudioParameterBool*>(p->getParameter(boom::ParamIDs::useDotted));
        param->operator=(!param->get()); // flip
        updateToggleArt(dottedToggle, param->get());
    };

    // Initial state
    updateToggleArt(tripletsToggle, dynamic_cast<juce::AudioParameterBool*>(processor.apvts.getParameter(boom::ParamIDs::useTriplets))->get());
    updateToggleArt(dottedToggle, dynamic_cast<juce::AudioParameterBool*>(processor.apvts.getParameter(boom::ParamIDs::useDotted))->get());



    // Button actions
    rollsBtn.onClick = [this]
    {
        if (processor.getEngine() != boom::Engine::Drums) return; // Rolls only for Drums
        rolls = std::make_unique<RollsComponent>();
        rolls->onGenerate = [this](const juce::String& style) { auto pattern = processor.generateRollPattern(style, dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter(boom::ParamIDs::bars))->getIndex() == 0 ? 4 : 8); processor.setCurrentPattern(pattern); drumGrid.setPattern(pattern); };
        modal.reset(juce::DialogWindow::LaunchOptions().withTitle("ROLLS").withComponent(rolls.release(), true).withResizable(true, true).launchAsync());
    };

    bumpBtn.onClick = [this]
    {
        bumppit = std::make_unique<BumppitComponent>();
        bumppit->onBump = [this] { if (processor.getEngine() == boom::Engine::Drums) { processor.bumpDrumRowsUp(); drumGrid.setRows(processor.getDrumRows()); drumGrid.repaint(); } /* 808/Bass handled by key/scale change window later */ };
        modal.reset(juce::DialogWindow::LaunchOptions().withTitle("BUMPPIT").withComponent(bumppit.release(), true).withResizable(false, false).launchAsync());
    };

    flipBtn.onClick = [this]
    {
        flippit = std::make_unique<FlippitComponent>();
        flippit->onFlip = [this] { processor.flipCurrentPatternSlightly(); drumGrid.setPattern(processor.getCurrentPattern()); pianoRoll.setPattern(processor.getCurrentPattern()); };
        modal.reset(juce::DialogWindow::LaunchOptions().withTitle("FLIPPIT").withComponent(flippit.release(), true).withResizable(false, false).launchAsync());
    };

    aiBtn.onClick = [this]
    {
        aiTools = std::make_unique<AIToolsComponent>(processor.getEngine());
        aiTools->setListener(this);
        modal.reset(juce::DialogWindow::LaunchOptions().withTitle("A.I. TOOLS").withComponent(aiTools.release(), true).withResizable(true, true).launchAsync());
    };

    // Initial state
    syncEngineUI();
    drumGrid.setRows(processor.getDrumRows());
}

void BoomAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void BoomAudioProcessorEditor::resized()
{
    // Top engine row
    auto engineRow = r.removeFromTop(48);
    engineLblImg.setBounds(engineRow.removeFromLeft(140).reduced(0, 6)); // small label left
    engine808.setBounds(engineRow.removeFromLeft(110));
    engineBass.setBounds(engineRow.removeFromLeft(110));
    engineDrums.setBounds(engineRow.removeFromLeft(110));

    // Left-side labels near controls (tighten to your taste)
    keyLblImg.setBounds(left.removeFromTop(18));   keyBox.setBounds(left.removeFromTop(28));
    scaleLblImg.setBounds(left.removeFromTop(18)); scaleBox.setBounds(left.removeFromTop(28));
    timeSigLblImg.setBounds(left.removeFromTop(18)); timeSigBox.setBounds(left.removeFromTop(28));
    octaveLblImg.setBounds(left.removeFromTop(18)); octaveBox.setBounds(left.removeFromTop(28));
    styleLblImg.setBounds(left.removeFromTop(18));  bassStyleBox.setBounds(left.removeFromTop(28));
    restDensityLblImg.setBounds(left.removeFromTop(18)); restDensityDrums.setBounds(left.removeFromTop(24));
    barsLblImg.setBounds(left.removeFromTop(18));  barsBox.setBounds(left.removeFromTop(28));

    // Triplets/Dotted labels + toggle images + density sliders
    tripletsLblImg.setBounds(left.removeFromTop(18));
    tripletsToggle.setBounds(left.removeFromTop(24));
    tripletDensity.setBounds(left.removeFromTop(24));

    dottedNotesLblImg.setBounds(left.removeFromTop(18));
    dottedToggle.setBounds(left.removeFromTop(24));
    dottedDensity.setBounds(left.removeFromTop(24));

    // Humanize label + sliders
    humanizeLblImg.setBounds(left.removeFromTop(18));
    humanizeTiming.setBounds(left.removeFromTop(24));
    humanizeVelocity.setBounds(left.removeFromTop(24));
    swing.setBounds(left.removeFromTop(24));

    // Right-side window launchers
    aiToolsLblImg.setBounds(right.removeFromTop(22));
    aiBtn.setBounds(right.removeFromTop(42));
    rollsBtn.setBounds(right.removeFromTop(42));
    bumpBtn.setBounds(right.removeFromTop(42));
    flipBtn.setBounds(right.removeFromTop(42));

    // Logo bottom-left (or top center if you prefer)
    logoImg.setBounds(getLocalBounds().removeFromTop(64).removeFromRight(160).translated(12, 0));

    // Bottom buttons
    auto bottom = getLocalBounds().removeFromBottom(60).reduced(12);
    generateBtn.setBounds(bottom.removeFromLeft(240));
    dragMidiBtn.setBounds(bottom.removeFromRight(240));

    syncEngineUI();
};

void BoomAudioProcessorEditor::syncEngineUI()
{
    auto engine = processor.getEngine();

    // Visibility and mode toggles per engine
    const bool isDrums = engine == boom::Engine::Drums;

    drumGrid.setVisible(isDrums);
    pianoRoll.setVisible(!isDrums);
    rollsBtn.setVisible(isDrums);

    // Show/Hide engine-specific controls
    keyBox.setVisible(engine == boom::Engine::e808);
    scaleBox.setVisible(engine == boom::Engine::e808);
    octaveBox.setVisible(engine == boom::Engine::e808);
    restDensity808.setVisible(engine == boom::Engine::e808);

    bassStyleBox.setVisible(engine == boom::Engine::Bass);

    drumStyleBox.setVisible(isDrums);
    restDensityDrums.setVisible(isDrums);
}

void BoomAudioProcessorEditor::requestApplyAI(const juce::String& op)
{
    if (op == "Rhythmimick") {
        // TODO: Trigger analysis pipeline (tempo detection, bar alignment, drum onset mapping)
    }
    else if (op == "SlapSmith") {
        // TODO: Read mini-seed grid inside AI window, generate embellishments and update processor pattern
    }
    else if (op == "StyleBlender") {
        // TODO: Blend two styles into a new pattern according to styleBlendKnob
    }
    else if (op == "BeatBox") {
        // TODO: VAD record ? onset detect ? map to drum lanes
    }
}
