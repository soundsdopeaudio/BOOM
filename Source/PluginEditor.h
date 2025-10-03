#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"
#include "Components/DrumGridComponent.h"
#include "Components/PianoRollComponent.h"
#include "Components/AITools/AIToolsComponent.h"
#include "Components/ModalWindows/BumppitComponent.h"
#include "Components/ModalWindows/FlippitComponent.h"
#include "Components/ModalWindows/RollsComponent.h"

class BoomAudioProcessorEditor : public juce::AudioProcessorEditor,
                                 private AIToolsComponent::Listener,
                                 private juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit BoomAudioProcessorEditor (BoomAudioProcessor&);
    ~BoomAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void requestApplyAI(const juce::String& op) override;
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    void syncEngineUI();

    BoomAudioProcessor& processor;

    // Engine buttons
    juce::TextButton engine808, engineBass, engineDrums;

    // Shared controls
    juce::ComboBox timeSigBox, barsBox;
    juce::Slider humanizeTiming;
    juce::Slider humanizeVelocity;
    juce::Slider swing;

    juce::ToggleButton tripletsToggle, dottedToggle;
    juce::Slider tripletDensity;
    juce::Slider dottedDensity;

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

    // Generate + Drag MIDI
    juce::TextButton generateBtn, dragMidiBtn;

    // Grid area: either PianoRoll or DrumGrid
    DrumGridComponent drumGrid;
    PianoRollComponent pianoRoll;

    // Modal/secondary windows
    juce::TextButton rollsBtn, aiBtn, bumpBtn, flipBtn;

    std::unique_ptr<juce::DialogWindow> modal;
    std::unique_ptr<BumppitComponent> bumppit;
    std::unique_ptr<FlippitComponent> flippit;
    std::unique_ptr<RollsComponent> rolls;
    std::unique_ptr<AIToolsComponent> aiTools;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BoomAudioProcessorEditor)
};