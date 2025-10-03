#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class BumppitComponent : public juce::Component
{
public:
    std::function<void()> onBump;
    juce::ImageButton bumpButton { "BUMP" }, diceButton{ "Dice" };

    BumppitComponent()
    {
        addAndMakeVisible(bumpButton);
        addAndMakeVisible(diceButton);
        bumpButton.onClick = [this] { if (onBump) onBump(); };
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(12);
        bumpButton.setBounds(r.removeFromTop(32));
        diceButton.setBounds(r.removeFromTop(28));
    }
};