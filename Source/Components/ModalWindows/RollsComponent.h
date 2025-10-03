// =============================================================
// FILE: Source/components/ModalWindows/RollsComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "EngineDefs.h"

class RollsComponent : public juce::Component
{
public:

    juce::ComboBox styleBox;

    std::function<void(const juce::String& style)> onGenerate;

    RollsComponent()
    {
        styleBox.addItemList(boom::styleChoices, 1);
        addAndMakeVisible(styleBox);
        addAndMakeVisible(generateBtn);
        addAndMakeVisible(diceBtn);
        generateBtn.onClick = [this] { if (onGenerate) onGenerate(styleBox.getText()); };
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(12);
        styleBox.setBounds(r.removeFromTop(28));
        generateBtn.setBounds(r.removeFromTop(32));
        diceBtn.setBounds(r.removeFromTop(28));
    }

private:

    juce::ComboBox styleBox;
    juce::ImageButton generateBtn;
    juce::ImageButton diceBtn;

}