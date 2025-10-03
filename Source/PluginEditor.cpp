#include "PluginEditor.h"
#include "Skin.h"
#include "../Source/Components/ModalWindows/BumppitComponent.h"

BoomAudioProcessorEditor::BoomAudioProcessorEditor (BoomAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p),
      engine808("808"), engineBass("BASS"), engineDrums("DRUMS"),
      humanizeTiming(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight),
      humanizeVelocity(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight),
      swing(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight),
      tripletsToggle("Triplets"), dottedToggle("Dotted"),
      tripletDensity(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight),
      dottedDensity(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight),
      restDensity808(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight),
      restDensityDrums(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight),
      generateBtn("GENERATE"), dragMidiBtn("DRAG MIDI"),
      drumGrid(p), pianoRoll(p),
      rollsBtn("ROLLS"), aiBtn("A.I. TOOLS"), bumpBtn("BUMPPIT"), flipBtn("FLIPPIT")
{
    setResizable(true, true); setSize (980, 640);

    // === Engine selectors ===
    auto engineChange = [this](boom::Engine e){ processor.apvts.getParameter(boom::ParamIDs::engine)->beginChangeGesture();
        dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter(boom::ParamIDs::engine))->operator=((int)e);
        processor.apvts.getParameter(boom::ParamIDs::engine)->endChangeGesture();
        syncEngineUI(); };

    engine808.onClick = [engineChange]{ engineChange(boom::Engine::e808); };
    engineBass.onClick = [engineChange]{ engineChange(boom::Engine::Bass); };
    engineDrums.onClick= [engineChange]{ engineChange(boom::Engine::Drums); };

    addAndMakeVisible(engine808); addAndMakeVisible(engineBass); addAndMakeVisible(engineDrums);

    // === Shared ===
    addAndMakeVisible(timeSigBox); timeSigBox.addItemList(boom::getTimeSigChoices(), 1);
    addAndMakeVisible(barsBox); barsBox.addItemList(boom::getBarsChoices(), 1);
    timeSigAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::timeSig, timeSigBox);
    barsAtt    = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::bars, barsBox);

    addAndMakeVisible(humanizeTiming); addAndMakeVisible(humanizeVelocity); addAndMakeVisible(swing);
    humanizeTimingAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::humanizeTiming, humanizeTiming);
    humanizeVelocityAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::humanizeVelocity, humanizeVelocity);
    swingAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::swing, swing);

    addAndMakeVisible(tripletsToggle); addAndMakeVisible(dottedToggle);
    tripletsAtt = std::make_unique<ButtonAttachment>(processor.apvts, boom::ParamIDs::useTriplets, tripletsToggle);
    dottedAtt   = std::make_unique<ButtonAttachment>(processor.apvts, boom::ParamIDs::useDotted, dottedToggle);
    addAndMakeVisible(tripletDensity); addAndMakeVisible(dottedDensity);
    tripletDensityAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::tripletDensity, tripletDensity);
    dottedDensityAtt  = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::dottedDensity, dottedDensity);

    // === 808 ===
    addAndMakeVisible(keyBox); keyBox.addItemList(boom::getKeyChoices(), 1);
    addAndMakeVisible(scaleBox); scaleBox.addItemList(boom::getScaleChoices(), 1);
    addAndMakeVisible(octaveBox); octaveBox.addItemList(boom::getOctaveChoices(), 1);
    addAndMakeVisible(restDensity808);
    keyAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::key, keyBox);
    scaleAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::scale, scaleBox);
    octaveAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::octave, octaveBox);
    rest808Att = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::restDensity, restDensity808);

    // === Bass ===
    addAndMakeVisible(bassStyleBox); bassStyleBox.addItemList(boom::getStyleChoices(), 1);
    bassStyleAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::bassStyle, bassStyleBox);

    // === Drums ===
    addAndMakeVisible(drumStyleBox); drumStyleBox.addItemList(boom::getStyleChoices(), 1);
    addAndMakeVisible(restDensityDrums);
    drumStyleAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::drumStyle, drumStyleBox);
    restDrumsAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::drumRestDensity, restDensityDrums);

    // === Grid area ===
    addAndMakeVisible(drumGrid); addAndMakeVisible(pianoRoll);

    // === Action buttons ===
    addAndMakeVisible(generateBtn); addAndMakeVisible(dragMidiBtn);

    // Modal/window launchers
    addAndMakeVisible(rollsBtn); addAndMakeVisible(aiBtn); addAndMakeVisible(bumpBtn); addAndMakeVisible(flipBtn);

    // Button actions
    rollsBtn.onClick = [this]
    {
        if (processor.getEngine() != boom::Engine::Drums) return; // Rolls only for Drums
        rolls = std::make_unique<RollsComponent>();
        rolls->onGenerate = [this](const juce::String& style){ auto pattern = processor.generateRollPattern(style, dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter(boom::ParamIDs::bars))->getIndex()==0?4:8); processor.setCurrentPattern(pattern); drumGrid.setPattern(pattern); };
        modal.reset(juce::DialogWindow::LaunchOptions().withTitle("ROLLS").withComponent(rolls.release(), true).withResizable(true, true).launchAsync());
    };

    bumpBtn.onClick = [this]
    {
        bumppit = std::make_unique<BumppitComponent>();
        bumppit->onBump = [this]{ if (processor.getEngine()==boom::Engine::Drums){ processor.bumpDrumRowsUp(); drumGrid.setRows(processor.getDrumRows()); drumGrid.repaint(); } /* 808/Bass handled by key/scale change window later */ };
        modal.reset(juce::DialogWindow::LaunchOptions().withTitle("BUMPPIT").withComponent(bumppit.release(), true).withResizable(false, false).launchAsync());
    };

    flipBtn.onClick = [this]
    {
        flippit = std::make_unique<FlippitComponent>();
        flippit->onFlip = [this]{ processor.flipCurrentPatternSlightly(); drumGrid.setPattern(processor.getCurrentPattern()); pianoRoll.setPattern(processor.getCurrentPattern()); };
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

BoomAudioProcessorEditor::~BoomAudioProcessorEditor()
{
    processor.apvts.removeParameterListener(boom::ParamIDs::useTriplets, this);
    processor.apvts.removeParameterListener(boom::ParamIDs::useDotted, this);
}

void BoomAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void BoomAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(12);

    // top engine row
    auto engineRow = r.removeFromTop(36);
    engine808.setBounds(engineRow.removeFromLeft(90)); engineBass.setBounds(engineRow.removeFromLeft(90)); engineDrums.setBounds(engineRow.removeFromLeft(90));

    // left-side controls
    auto left = r.removeFromLeft(260);
    timeSigBox.setBounds(left.removeFromTop(28));
    barsBox.setBounds(left.removeFromTop(28));

    tripletsToggle.setBounds(left.removeFromTop(24));
    tripletDensity.setBounds(left.removeFromTop(24));
    dottedToggle.setBounds(left.removeFromTop(24));
    dottedDensity.setBounds(left.removeFromTop(24));

    humanizeTiming.setBounds(left.removeFromTop(24));
    humanizeVelocity.setBounds(left.removeFromTop(24));
    swing.setBounds(left.removeFromTop(24));

    // 808 block
    keyBox.setBounds(left.removeFromTop(28));
    scaleBox.setBounds(left.removeFromTop(28));
    octaveBox.setBounds(left.removeFromTop(28));
    restDensity808.setBounds(left.removeFromTop(24));

    // Bass block
    bassStyleBox.setBounds(left.removeFromTop(28));

    // Drums block
    drumStyleBox.setBounds(left.removeFromTop(28));
    restDensityDrums.setBounds(left.removeFromTop(24));

    // right side controls (windows + generate)
    auto right = r.removeFromRight(220);
    rollsBtn.setBounds(right.removeFromTop(36));
    aiBtn.setBounds(right.removeFromTop(36));
    bumpBtn.setBounds(right.removeFromTop(36));
    flipBtn.setBounds(right.removeFromTop(36));

    // center grid
    auto gridArea = r.reduced(8);
    drumGrid.setBounds(gridArea);
    pianoRoll.setBounds(gridArea);

    // bottom buttons
    auto bottom = getLocalBounds().removeFromBottom(52).reduced(12);
    generateBtn.setBounds(bottom.removeFromLeft(220));
    dragMidiBtn.setBounds(bottom.removeFromRight(220));

    syncEngineUI();
}

void BoomAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    // When state changes via APVTS, update images to On/Off accordingly
    auto updateToggleArt = [&](juce::ToggleButton& b, bool isOn)
    {
        // Placeholder for image-based toggle button updates
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
    } else if (op == "SlapSmith") {
        // TODO: Read mini-seed grid inside AI window, generate embellishments and update processor pattern
    } else if (op == "StyleBlender") {
        // TODO: Blend two styles into a new pattern according to styleBlendKnob
    } else if (op == "BeatBox") {
        // TODO: VAD record → onset detect → map to drum lanes
    }
}