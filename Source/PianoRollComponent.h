// =============================================================
// FILE: Source/components/PianoRollComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class PianoRollComponent : public juce::Component
{
public:
    explicit PianoRollComponent(BoomAudioProcessor& p) : processor(p) {}

    void setPattern(const BoomAudioProcessor::Pattern& pat) { pattern = pat; repaint(); }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour::fromRGB(24, 24, 24));
        auto r = getLocalBounds();
        const int rows = 24; // 2 octaves
        const int cols = 64; // 4 bars x 16th
        const auto cellW = r.getWidth() / (float)cols;
        const auto cellH = r.getHeight() / (float)rows;

        // grid
        g.setColour(juce::Colours::dimgrey);
        for (int c = 0; c <= cols; ++c)
            g.drawVerticalLine((int)std::round(c * cellW), 0.f, (float)r.getHeight());
        for (int rr = 0; rr <= rows; ++rr)
            g.drawHorizontalLine((int)std::round(rr * cellH), 0.f, (float)r.getWidth());

        // notes
        g.setColour(juce::Colours::purple);
        for (auto& n : pattern)
        {
            int col = (n.startTick / 24) % 64;
            int row = juce::jlimit(0, rows - 1, rows - 1 - ((n.pitch - 36) % rows));
            g.fillRoundedRectangle(juce::Rectangle<float>(col * cellW + 2, row * cellH + 2, cellW * juce::jmax(1, n.lengthTicks / 24) - 4, cellH - 4), 4.f);
        }
    }

private:
    BoomAudioProcessor& processor;
    BoomAudioProcessor::Pattern pattern;
};
