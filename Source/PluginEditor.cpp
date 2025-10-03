// =============================================================
// FILE: Source/PluginEditor.cpp
// =============================================================
#include "PluginEditor.h"

BoomAudioProcessorEditor::BoomAudioProcessorEditor(BoomAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p), drumGrid(p), pianoRoll(p)
{
    setResizable(true, true); setSize(980, 640);

    // Make all controls visible
    addAndMakeVisible(engine808);
    addAndMakeVisible(engineBass);
    addAndMakeVisible(engineDrums);
    addAndMakeVisible(generateBtn);
    addAndMakeVisible(dragMidiBtn);
    addAndMakeVisible(rollsBtn);
    addAndMakeVisible(aiBtn);
    addAndMakeVisible(bumpBtn);
    addAndMakeVisible(flipBtn);
    addAndMakeVisible(logoImg);
    addAndMakeVisible(engineLblImg);
    addAndMakeVisible(humanizeLblImg);
    addAndMakeVisible(keyLblImg);
    addAndMakeVisible(scaleLblImg);
    addAndMakeVisible(timeSigLblImg);
    addAndMakeVisible(octaveLblImg);
    addAndMakeVisible(styleLblImg);
    addAndMakeVisible(restDensityLblImg);
    addAndMakeVisible(barsLblImg);
    addAndMakeVisible(drumsLblImg);
    addAndMakeVisible(drumsSelectorLblImg);
    addAndMakeVisible(bassSelectorLblImg);
    addAndMakeVisible(_808BassLblImg);
    addAndMakeVisible(aiToolsLblImg);
    addAndMakeVisible(tripletsLblImg);
    addAndMakeVisible(dottedNotesLblImg);
    addAndMakeVisible(tripletsToggle);
    addAndMakeVisible(dottedToggle);
    addAndMakeVisible(timeSigBox);
    addAndMakeVisible(barsBox);
    addAndMakeVisible(humanizeTiming);
    addAndMakeVisible(humanizeVelocity);
    addAndMakeVisible(swing);
    addAndMakeVisible(tripletDensity);
    addAndMakeVisible(dottedDensity);
    addAndMakeVisible(keyBox);
    addAndMakeVisible(scaleBox);
    addAndMakeVisible(octaveBox);
    addAndMakeVisible(restDensity808);
    addAndMakeVisible(bassStyleBox);
    addAndMakeVisible(drumStyleBox);
    addAndMakeVisible(restDensityDrums);

    // Populate ComboBoxes
    timeSigBox.addItemList(boom::getTimeSigChoices(), 1);
    barsBox.addItemList(boom::getBarsChoices(), 1);
    keyBox.addItemList(boom::getKeyChoices(), 1);
    scaleBox.addItemList(boom::getScaleChoices(), 1);
    octaveBox.addItemList(boom::getOctaveChoices(), 1);
    bassStyleBox.addItemList(boom::getStyleChoices(), 1);
    drumStyleBox.addItemList(boom::getStyleChoices(), 1);

    // APVTS Attachments
    timeSigAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::timeSig, timeSigBox);
    barsAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::bars, barsBox);
    keyAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::key, keyBox);
    scaleAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::scale, scaleBox);
    octaveAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::octave, octaveBox);
    bassStyleAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::bassStyle, bassStyleBox);
    drumStyleAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::drumStyle, drumStyleBox);
    humanizeTimingAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::humanizeTiming, humanizeTiming);
    humanizeVelocityAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::humanizeVelocity, humanizeVelocity);
    swingAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::swing, swing);
    rest808Att = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::restDensity, restDensity808);
    restDrumsAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::drumRestDensity, restDensityDrums);
    tripletDensityAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::tripletDensity, tripletDensity);
    dottedDensityAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::dottedDensity, dottedDensity);
    tripletsAtt = std::make_unique<ButtonAttachment>(processor.apvts, boom::ParamIDs::useTriplets, tripletsToggle);
    dottedAtt = std::make_unique<ButtonAttachment>(processor.apvts, boom::ParamIDs::useDotted, dottedToggle);

    // Listen for toggle changes to update artwork
    processor.apvts.addParameterListener(boom::ParamIDs::useTriplets, this);
    processor.apvts.addParameterListener(boom::ParamIDs::useDotted, this);


    // === Engine selectors ===
    auto engineChange = [this](boom::Engine e) { processor.apvts.getParameter(boom::ParamIDs::engine)->beginChangeGesture();
    dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter(boom::ParamIDs::engine))->operator=((int)e);
    processor.apvts.getParameter(boom::ParamIDs::engine)->endChangeGesture();
    syncEngineUI(); };

    engine808.onClick = [engineChange] { engineChange(boom::Engine::e808); };
    engineBass.onClick = [engineChange] { engineChange(boom::Engine::Bass); };
    engineDrums.onClick = [engineChange] { engineChange(boom::Engine::Drums); };

    // Image assets
    setButtonImages3(engine808, "808Btn_png", "808Btn_hover_png", "808Btn_down_png");
    setButtonImages3(engineBass, "bassBtn_png", "bassBtn_hover_png", "bassBtn_down_png");
    setButtonImages3(engineDrums, "drumsBtn_png", "drumsBtn_hover_png", "drumsBtn_down_png");
    setButtonImages3(generateBtn, "generateBtn_png", "generateBtn_hover_png", "generateBtn_down_png");
    setButtonImages3(dragMidiBtn, "saveMidiBtn_png", "saveMidiBtn_hover_png", "saveMidiBtn_down_png");
    setButtonImages3(rollsBtn, "rollsBtn_png", "rollsBtn_hover_png", "rollsBtn_down_png");
    setButtonImages3(aiBtn, "aiToolsBtn_png", "aiToolsBtn_hover_png", "aiToolsBtn_down_png");
    setButtonImages3(bumpBtn, "bumppitBtn_png", "bumppitBtn_hover_png", "bumppitBtn_down_png");
    setButtonImages3(flipBtn, "flippitBtn_png", "flippitBtn_hover_png", "flippitBtn_down_png");
    logoImg.setImage(pngFromBinary("logo_png"));

    // Labels
    auto setLbl = [&](juce::ImageComponent& c, const char* name) { c.setImage(pngFromBinary(name)); };
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

    // Initial state for toggle buttons
    parameterChanged(boom::ParamIDs::useTriplets, *processor.apvts.getRawParameterValue(boom::ParamIDs::useTriplets));
    parameterChanged(boom::ParamIDs::useDotted, *processor.apvts.getRawParameterValue(boom::ParamIDs::useDotted));

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

    addAndMakeVisible(drumGrid);
    addAndMakeVisible(pianoRoll);
}

BoomAudioProcessorEditor::~BoomAudioProcessorEditor()
{
    processor.apvts.removeParameterListener(boom::ParamIDs::useTriplets, this);
    processor.apvts.removeParameterListener(boom::ParamIDs::useDotted, this);
}

void BoomAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void BoomAudioProcessorEditor::resized()
{
    auto r = getLocalBounds();

    // Define main layout areas
    auto topArea = r.removeFromTop(80);
    auto bottomArea = r.removeFromBottom(80);
    auto leftArea = r.removeFromLeft(200).reduced(10);
    auto rightArea = r.removeFromRight(200).reduced(10);
    auto mainGridArea = r;

    // Top Row: Logo and Engine selectors
    logoImg.setBounds(topArea.removeFromRight(180).reduced(10));
    auto engineRow = topArea;
    engineLblImg.setBounds(engineRow.removeFromLeft(140).reduced(0, 6));
    engine808.setBounds(engineRow.removeFromLeft(110));
    engineBass.setBounds(engineRow.removeFromLeft(110));
    engineDrums.setBounds(engineRow.removeFromLeft(110));

    // Bottom Row: Generate and Drag buttons
    generateBtn.setBounds(bottomArea.removeFromLeft(240).reduced(10));
    dragMidiBtn.setBounds(bottomArea.removeFromRight(240).reduced(10));

    // Left Column: Parameters
    keyLblImg.setBounds(leftArea.removeFromTop(18));        keyBox.setBounds(leftArea.removeFromTop(28));
    scaleLblImg.setBounds(leftArea.removeFromTop(18));      scaleBox.setBounds(leftArea.removeFromTop(28));
    timeSigLblImg.setBounds(leftArea.removeFromTop(18));    timeSigBox.setBounds(leftArea.removeFromTop(28));
    octaveLblImg.setBounds(leftArea.removeFromTop(18));     octaveBox.setBounds(leftArea.removeFromTop(28));
    barsLblImg.setBounds(leftArea.removeFromTop(18));       barsBox.setBounds(leftArea.removeFromTop(28));
    styleLblImg.setBounds(leftArea.removeFromTop(18));
    bassStyleBox.setBounds(leftArea.removeFromTop(28));
    drumStyleBox.setBounds(bassStyleBox.getBounds()); // Share bounds with bass style
    restDensityLblImg.setBounds(leftArea.removeFromTop(18));
    restDensity808.setBounds(leftArea.removeFromTop(24));
    restDensityDrums.setBounds(restDensity808.getBounds()); // Share bounds with 808 rest density

    // Right Column: Humanize, Note Types, and Tools
    humanizeLblImg.setBounds(rightArea.removeFromTop(18));
    humanizeTiming.setBounds(rightArea.removeFromTop(24));
    humanizeVelocity.setBounds(rightArea.removeFromTop(24));
    swing.setBounds(rightArea.removeFromTop(24));
    rightArea.removeFromTop(10); // spacer
    tripletsLblImg.setBounds(rightArea.removeFromTop(18));
    auto tripletLine = rightArea.removeFromTop(24);
    tripletsToggle.setBounds(tripletLine.removeFromLeft(30));
    tripletDensity.setBounds(tripletLine);
    dottedNotesLblImg.setBounds(rightArea.removeFromTop(18));
    auto dottedLine = rightArea.removeFromTop(24);
    dottedToggle.setBounds(dottedLine.removeFromLeft(30));
    dottedDensity.setBounds(dottedLine);
    rightArea.removeFromTop(10); // spacer
    aiToolsLblImg.setBounds(rightArea.removeFromTop(22));
    aiBtn.setBounds(rightArea.removeFromTop(42));
    rollsBtn.setBounds(rightArea.removeFromTop(42));
    bumpBtn.setBounds(rightArea.removeFromTop(42));
    flipBtn.setBounds(rightArea.removeFromTop(42));

    // Center Area: Grids
    drumGrid.setBounds(mainGridArea);
    pianoRoll.setBounds(mainGridArea);

    syncEngineUI();
}

void BoomAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    // When state changes via APVTS, update images to On/Off accordingly
    auto updateToggleArt = [&](juce::ImageButton& b, bool isOn)
    {
        if (isOn)
            setButtonImages3(b, "toggleBtnOn_png", "toggleBtnOn_hover_png", "toggleBtnOn_down_png");
        else
            setButtonImages3(b, "toggleBtnOff_png", "toggleBtnOff_hover_png", "toggleBtnOff_down_png");
    };

    if (parameterID == boom::ParamIDs::useTriplets)
    {
        updateToggleArt(tripletsToggle, newValue > 0.5f);
    }
    else if (parameterID == boom::ParamIDs::useDotted)
    {
        updateToggleArt(dottedToggle, newValue > 0.5f);
    }
}

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
