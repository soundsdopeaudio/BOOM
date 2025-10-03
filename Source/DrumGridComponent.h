// =============================================================
// FILE: Source/components/DrumGridComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class DrumGridComponent : public juce::Component
{
public:
    explicit DrumGridComponent(BoomAudioProcessor& p)
        : processor(p)
        , rowNames(boom::drumRows)
    {
    }

    void setPattern(const BoomAudioProcessor::Pattern& pat)
    {
        pattern = pat;
        repaint();
    }
    void setRows(const juce::StringArray& rows)
    {
        rowNames = rows;
        repaint();
    }

    std::function<void(int row, int tick)> onToggleCell; // UI callback to add/remove events

    void paint(juce::Graphics& g) override
    {
        // === BOOM THEME ===
        const juce::Colour bg = juce::Colour(0xFF092806); // grid background
        const juce::Colour gridLine = juce::Colour(0xFF2D2E41); // grid lines
        const juce::Colour noteCol = juce::Colour(0xFF7CD400); // notes
        const juce::Colour textCol = juce::Colour(0xFFC9D2A7); // labels (sparingly)
        const juce::Colour headerBg = juce::Colour(0xFF092806).brighter(0.06f);
        const juce::Colour barLine = juce::Colour(0xFFC9D2A7).withAlpha(0.35f);

        g.fillAll(bg);

        auto full = getLocalBounds();
        const int headerH = 22; // top header for bar numbers
        auto header = full.removeFromTop(headerH);
        auto r = full; // grid area below header

        const int rows = juce::jmax(1, rowNames.size());
        const int cols = 64; // 4 bars x 16 steps
        const auto cellW = r.getWidth() / (float)cols;
        const auto cellH = r.getHeight() / (float)rows;

        // ---- Header (bar numbers + thick bar lines) ----
        g.setColour(headerBg);
        g.fillRect(header);

        for (int bar = 0; bar <= 4; ++bar)
        {
            const int x = r.getX() + (int)std::round(bar * 16 * cellW);

            if (bar < 4) // label "1.1", "2.1", etc.
            {
                g.setColour(textCol);
                g.drawText(juce::String(bar + 1) + ".1",
                           x + 4,
                           header.getY() + 2,
                           48,
                           header.getHeight() - 4,
                           juce::Justification::left);
            }

            // thick bar separator through header + grid
            g.setColour(barLine);
            g.drawLine((float)x,
                       (float)header.getY(),
                       (float)x,
                       (float)(header.getBottom() + r.getHeight()),
                       2.0f);
        }

        // ---- Grid (beat & subdivision lines) ----
        g.setColour(gridLine);
        for (int c = 0; c <= cols; ++c)
        {
            const int x = r.getX() + (int)std::round(c * cellW);
            g.drawVerticalLine(x, (float)r.getY(), (float)r.getBottom());
        }
        for (int rr = 0; rr <= rows; ++rr)
        {
            const int y = r.getY() + (int)std::round(rr * cellH);
            g.drawHorizontalLine(y, (float)r.getX(), (float)r.getRight());
        }

        // ---- Lane labels (left edge of each row) ----
        g.setColour(textCol);
        for (int rr = 0; rr < rows; ++rr)
        {
            g.drawText(rowNames[rr],
                       r.getX() + 6,
                       (int)(r.getY() + rr * cellH) + 2,
                       juce::jmin(110, r.getWidth() / 6),
                       (int)cellH - 4,
                       juce::Justification::left);
        }

        // ---- Notes ----
        g.setColour(noteCol);
        for (auto& n : pattern)
        {
            const int stepsPerBar = 16;
            const int col = (n.startTick / 24) % (stepsPerBar * 4); // 24 ticks per 16th
            const int x = r.getX() + (int)std::round(col * cellW);
            const int y = r.getY() + (int)std::round(n.row * cellH);
            g.fillRoundedRectangle(juce::Rectangle<float>((float)x + 2, (float)y + 2, cellW - 4, cellH - 4),
                                   4.0f);
        }
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        const int rows = juce::jmax(1, rowNames.size());
        const int cols = 64;

        const int headerH = 22;
        auto r = getLocalBounds().withTrimmedTop(headerH); // ignore header clicks

        const auto cellW = r.getWidth() / (float)cols;
        const auto cellH = r.getHeight() / (float)rows;

        if (!r.contains(e.getPosition()))
            return;

        const int c = juce::jlimit(0, cols - 1, (int)((e.position.x - (float)r.getX()) / cellW));
        const int rr = juce::jlimit(0, rows - 1, (int)((e.position.y - (float)r.getY()) / cellH));

        if (onToggleCell)
            onToggleCell(rr, c * 24); // 24 ticks per 16th
    }

private:
    BoomAudioProcessor& processor;
    BoomAudioProcessor::Pattern pattern;
    juce::StringArray rowNames;
};