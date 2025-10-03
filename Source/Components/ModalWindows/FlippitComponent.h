// =============================================================
// FILE: Source/components/ModalWindows/FlippitComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class FlippitComponent : public juce::Component
{
public:
    std::function<void()> onFlip;
    juce::TextButton flipButton { "FLIPPIT" }, randomButton{ "Dice" };

    FlippitComponent()
    {
        addAndMakeVisible(flipButton);
        addAndMakeVisible(randomButton);
        flipButton.onClick = [this] { if (onFlip) onFlip(); };
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(12);
        flipButton.setBounds(r.removeFromTop(32));
        randomButton.setBounds(r.removeFromTop(28));
    }
}; #pragma once
