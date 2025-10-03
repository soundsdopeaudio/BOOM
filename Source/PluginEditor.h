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
#include "ui/Skin.h"

class BoomAudioProcessorEditor : public juce::AudioProcessorEditor,
    private AIToolsComponent::Listener
{
public:
    explicit BoomAudioProcessorEditor(BoomAudioProcessor&);
    ~BoomAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // === AIToolsComponent::Listener ===
    void requestApplyAI(const juce::String& op) override;

    void syncEngineUI();

    BoomAudioProcessor& processor;


    // Shared controls
    juce::ComboBox timeSigBox, barsBox;
    juce::Slider humanizeTiming { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };
    juce::Slider humanizeVelocity { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };
    juce::Slider swing { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

    juce::ImageButton tripletsToggle, dottedToggle;
    juce::Slider tripletDensity { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };
    juce::Slider dottedDensity { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

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
    juce::Slider restDensity808 { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

    // Bass specific
    juce::ComboBox bassStyleBox;

    // Drums specific
    juce::ComboBox drumStyleBox;
    juce::Slider restDensityDrums { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoomAudioProcessorEditor)
}; #pragma once
