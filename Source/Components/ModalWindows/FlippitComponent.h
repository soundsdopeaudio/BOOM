// =============================================================
// FILE: Source/components/ModalWindows/FlippitComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class FlippitComponent : public juce::Component
{
public:
    std::function<void()> onFlip;
    juce::ImageButton diceBtn;
    juce::ImageButton flippitBtn;

    FlippitComponent()
    {
        addAndMakeVisible(flippitBtn);
        addAndMakeVisible(diceBtn);
        flippitBtn.onClick = [this] { if (onFlip) onFlip(); };
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(12);
        flippitBtn.setBounds(r.removeFromTop(32));
        diceBtn.setBounds(r.removeFromTop(28));
    }
};