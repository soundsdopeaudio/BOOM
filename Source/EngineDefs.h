// =============================================================
// FILE: Source/EngineDefs.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace boom
{
    enum class Engine : int { e808 = 0, Bass = 1, Drums = 2 };

    inline const juce::StringArray& getEngineChoices()
    {
        static const juce::StringArray choices { "808", "Bass", "Drums" };
        return choices;
    }

    // Canonical drum lanes (rows) for the grid
    inline const juce::StringArray& getDefaultDrumRows()
    {
        static const juce::StringArray rows {
            "Kick", "Snare/Clap", "Hi-Hat", "Open Hat", "Perc 1", "Perc 2", "Perc 3"
        };
        return rows;
    }

    // Style options used by Bass + Drums + Style Blender
    inline const juce::StringArray& getStyleChoices()
    {
        static const juce::StringArray choices {
            "trap", "drill", "hip hop", "rock", "edm", "reggaeton", "r&b", "pop", "wxstie"
        };
        return choices;
    }

    // Music theory choices (shared with BANG)
    inline const juce::StringArray& getKeyChoices()
    {
        static const juce::StringArray choices { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        return choices;
    }

    inline const juce::StringArray& getScaleChoices()
    {
        static const juce::StringArray choices {
            // keep aligned with BANG
            "Major", "Natural Minor", "Harmonic Minor", "Melodic Minor",
                "Dorian", "Phrygian", "Lydian", "Mixolydian", "Locrian",
                "Harmonic Minor Modes", "Melodic Minor Modes",
                "Spanish 8-Tone", "Spanish Gypsy"
        };
        return choices;
    }

    inline const juce::StringArray& getTimeSigChoices()
    {
        static const juce::StringArray choices { "4/4", "3/4", "6/8", "7/8", "5/4", "9/8", "12/8", "2/4" };
        return choices;
    }

    inline const juce::StringArray& getBarsChoices()
    {
        static const juce::StringArray choices { "4", "8" };
        return choices;
    }

    inline const juce::StringArray& getOctaveChoices()
    {
        static const juce::StringArray choices { "-2", "-1", "0", "+1", "+2" };
        return choices;
    }
}