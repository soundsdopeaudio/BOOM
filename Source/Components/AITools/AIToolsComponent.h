// =============================================================
// FILE: Source/components/AITools/AIToolsComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "EngineDefs.h"

class AIToolsComponent : public juce::Component
{
public:
    struct Listener { virtual ~Listener() = default; virtual void requestApplyAI(const juce::String& op) = 0; };

    explicit AIToolsComponent(boom::Engine initialEngine) : currentEngine(initialEngine)
    {
        // Engine selector (applies to all AI tools)
        engineBox.addItemList(boom::engineChoices, 1); engineBox.setSelectedId((int)initialEngine + 1);
        addAndMakeVisible(engineBox);

        // Rhythmimick
        addAndMakeVisible(recordRhythmimick); recordRhythmimick.setButtonText("Record");
        addAndMakeVisible(rhythmimickPosition); rhythmimickPosition.setRange(0.0, 1.0, 0.0001);
        addAndMakeVisible(applyRhythmimick); applyRhythmimick.setButtonText("Generate from Recording");

        // SlapSmith (mini grid)
        addAndMakeVisible(slapSmithLabel); slapSmithLabel.setText("SlapSmith (seed grid)", juce::dontSendNotification);
        addAndMakeVisible(applySlapSmith); applySlapSmith.setButtonText("Embellish");

        // Style Blender
        addAndMakeVisible(styleABox); styleABox.addItemList(boom::styleChoices, 1); styleABox.setSelectedId(1);
        addAndMakeVisible(styleBBox); styleBBox.addItemList(boom::styleChoices, 1); styleBBox.setSelectedId(2);
        addAndMakeVisible(styleBlendKnob); styleBlendKnob.setRange(0.0, 1.0, 0.0001);
        addAndMakeVisible(applyStyleBlend); applyStyleBlend.setButtonText("Blend Styles");

        // BeatBox
        addAndMakeVisible(recordBeatBox); recordBeatBox.setButtonText("Record Beatbox");
        addAndMakeVisible(beatboxPosition); beatboxPosition.setRange(0.0, 1.0, 0.0001);
        addAndMakeVisible(applyBeatBox); applyBeatBox.setButtonText("Transcribe");

        // Wire callbacks (host signals upward)
        applyRhythmimick.onClick = [this] { if (listener) listener->requestApplyAI("Rhythmimick"); };
        applySlapSmith.onClick = [this] { if (listener) listener->requestApplyAI("SlapSmith"); };
        applyStyleBlend.onClick = [this] { if (listener) listener->requestApplyAI("StyleBlender"); };
        applyBeatBox.onClick = [this] { if (listener) listener->requestApplyAI("BeatBox"); };
    }

    void setListener(Listener* l) { listener = l; }
    void setEngine(boom::Engine e) { currentEngine = e; engineBox.setSelectedId((int)e + 1); }

    void resized() override
    {
        auto r = getLocalBounds().reduced(8);
        auto top = r.removeFromTop(28); engineBox.setBounds(top.removeFromLeft(160));

        auto col = r; col.removeFromRight(r.getWidth() / 2);
        auto col2 = r;

        // Rhythmimick block
        auto block = col.removeFromTop(120); recordRhythmimick.setBounds(block.removeFromTop(28));
        rhythmimickPosition.setBounds(block.removeFromTop(24));
        applyRhythmimick.setBounds(block.removeFromTop(28));

        // SlapSmith block
        block = col.removeFromTop(140);
        slapSmithLabel.setBounds(block.removeFromTop(22));
        applySlapSmith.setBounds(block.removeFromTop(28));

        // Style Blender block
        block = col2.removeFromTop(140);
        styleABox.setBounds(block.removeFromTop(28));
        styleBBox.setBounds(block.removeFromTop(28));
        styleBlendKnob.setBounds(block.removeFromTop(24));
        applyStyleBlend.setBounds(block.removeFromTop(28));

        // BeatBox block
        block = col2.removeFromTop(120);
        recordBeatBox.setBounds(block.removeFromTop(28));
        beatboxPosition.setBounds(block.removeFromTop(24));
        applyBeatBox.setBounds(block.removeFromTop(28));
    }

    // Expose some settings to editor if needed later
    juce::ComboBox engineBox, styleABox, styleBBox;
    juce::Slider styleBlendKnob { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

private:
    boom::Engine currentEngine;

    // Rhythmimick
    juce::TextButton recordRhythmimick, applyRhythmimick;
    juce::Slider rhythmimickPosition { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

    // SlapSmith
    juce::Label slapSmithLabel;
    juce::TextButton applySlapSmith;

    // Style Blender
    juce::TextButton applyStyleBlend;

    // BeatBox
    juce::TextButton recordBeatBox, applyBeatBox;
    juce::Slider beatboxPosition { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

    Listener* listener{ nullptr };
}; #pragma once
