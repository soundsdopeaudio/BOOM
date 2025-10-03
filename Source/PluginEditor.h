// =============================================================
// FILE: Source/PluginEditor.h
// =============================================================
#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"
#include "components/DrumGridComponent.h"
#include "components/PianoRollComponent.h"
#include "components/AITools/AIToolsComponent.h"
#include "components/ModalWindows/BumppitComponent.h"
#include "components/ModalWindows/FlippitComponent.h"
#include "components/ModalWindows/RollsComponent.h"
#include "Skin.h"

class BoomAudioProcessorEditor : public juce::AudioProcessorEditor,
    private AIToolsComponent::Listener,
    private juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit BoomAudioProcessorEditor(BoomAudioProcessor&);
    ~BoomAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // === AIToolsComponent::Listener ===
    void requestApplyAI(const juce::String& op) override
    {};

    // === juce::AudioProcessorValueTreeState::Listener ===
    void parameterChanged(const juce::String& parameterID, float newValue) override {};

    void syncEngineUI();

    BoomAudioProcessor& processor;


    // Shared controls
    juce::ComboBox timeSigBox, barsBox;
    juce::Slider humanizeTiming;
    juce::Slider humanizeVelocity;
    juce::Slider swing;

    juce::ImageButton tripletsToggle, dottedToggle;
    juce::Slider tripletDensity;
    juce::Slider dottedDensity;

    juce::ImageButton engine808, engineBass, engineDrums;
    juce::ImageButton generateBtn, dragMidiBtn;
    juce::ImageButton rollsBtn, aiBtn, bumpBtn, flipBtn;

    // Logo + label images
    juce::ImageComponent logoImg;
    juce::ImageComponent engineLblImg, humanizeLblImg, keyLblImg, scaleLblImg, timeSigLblImg,
        octaveLblImg, styleLblImg, restDensityLblImg, barsLblImg,
        drumsLblImg, drumsSelectorLblImg, bassSelectorLblImg, _808BassLblImg,
        aiToolsLblImg, tripletsLblImg, dottedNotesLblImg;

    // 808 specific
    juce::ComboBox keyBox, scaleBox, octaveBox;
    juce::Slider restDensity808;

    // Bass specific
    juce::ComboBox bassStyleBox;

    // Drums specific
    juce::ComboBox drumStyleBox;
    juce::Slider restDensityDrums;

    // Attachments
    using Attachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<Attachment> timeSigAtt, barsAtt, keyAtt, scaleAtt, octaveAtt, bassStyleAtt, drumStyleAtt;
    std::unique_ptr<SliderAttachment> humanizeTimingAtt, humanizeVelocityAtt, swingAtt, rest808Att, restDrumsAtt, tripletDensityAtt, dottedDensityAtt;
    std::unique_ptr<ButtonAttachment> tripletsAtt, dottedAtt;


    // Grid area: either PianoRoll or DrumGrid
    DrumGridComponent drumGrid;
    PianoRollComponent pianoRoll;


    std::unique_ptr<juce::DialogWindow> modal;
    std::unique_ptr<BumppitComponent> bumppit;
    std::unique_ptr<FlippitComponent> flippit;
    std::unique_ptr<RollsComponent> rolls;
    std::unique_ptr<AIToolsComponent> aiTools;

}