#pragma once
#include <JuceHeader.h>

// Helper to load PNG from BinaryData by name
inline juce::Image pngFromBinary(const juce::String& name)
{
    int dataSize = 0;
    const char* resourceData = BinaryData::getNamedResource(name.toRawUTF8(), dataSize);
    jassert(resourceData != nullptr && dataSize > 0);
    return juce::ImageFileFormat::loadFrom(resourceData, (size_t)dataSize);
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

    b.setImages(imgUp, imgOver, imgDown);
}