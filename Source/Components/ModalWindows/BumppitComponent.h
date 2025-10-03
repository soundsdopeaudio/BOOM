// =============================================================
// FILE: Source/components/ModalWindows/BumppitComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class BumppitComponent : public juce::Component
{
public:
    std::function<void()> onBump;
    juce::TextButton bumpButton { "BUMP" }, randomButton{ "Dice" };

    BumppitComponent()
    {
        addAndMakeVisible(bumpButton);
        addAndMakeVisible(randomButton);
        bumpButton.onClick = [this] { if (onBump) onBump(); };
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(12);
        bumpButton.setBounds(r.removeFromTop(32));
        randomButton.setBounds(r.removeFromTop(28));
    }
};