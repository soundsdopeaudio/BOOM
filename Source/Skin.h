#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

// Helper to load PNG from BinaryData by name
inline juce::Image pngFromBinary(const juce::String& name)
{
    auto* data = BinaryData::getNamedResource(name.toRawUTF8(), nullptr);
    int   length = 0;
    (void)BinaryData::getNamedResource(name.toRawUTF8(), &length);
    jassert(data != nullptr && length > 0);
    return juce::ImageFileFormat::loadFrom(data, (size_t)length);
}

// Set 3-state images on an ImageButton
inline void setButtonImages3(juce::ImageButton& b,
    const juce::String& up,
    const juce::String& over,
    const juce::String& down)
{
    auto imgUp = pngFromBinary(up);
    auto imgOver = pngFromBinary(over);
    auto imgDown = pngFromBinary(down);

    b.setImages(/*resizeButton*/ true, /*preserveProportions*/ true, /*animate*/ false,
        imgUp, 1.0f, juce::Colours::transparentBlack,
        imgOver, 1.0f, juce::Colours::transparentBlack,
        imgDown, 1.0f, juce::Colours::transparentBlack,
        nullptr, 1.0f, juce::Colours::transparentBlack);
}

