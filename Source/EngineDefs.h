
// =============================================================
// FILE: Source/EngineDefs.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace boom
{
    enum class Engine : int { e808 = 0, Bass = 1, Drums = 2 };

    static inline const juce::StringArray engineChoices { "808", "Bass", "Drums" };

    // Canonical drum lanes (rows) for the grid
    static inline const juce::StringArray defaultDrumRows {
        "Kick", "Snare/Clap", "Hi-Hat", "Open Hat", "Perc 1", "Perc 2", "Perc 3"
    };

    // Style options used by Bass + Drums + Style Blender
    static inline const juce::StringArray styleChoices {
        "trap", "drill", "hip hop", "rock", "edm", "reggaeton", "r&b", "pop", "wxstie"
    };

    // Music theory choices (shared with BANG)
    static inline const juce::StringArray keyChoices { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

    static inline const juce::StringArray scaleChoices {
        // keep aligned with BANG
        "Major", "Natural Minor", "Harmonic Minor", "Melodic Minor",
            "Dorian", "Phrygian", "Lydian", "Mixolydian", "Locrian",
            "Harmonic Minor Modes", "Melodic Minor Modes",
            "Spanish 8-Tone", "Spanish Gypsy"
    };

    static inline const juce::StringArray timeSigChoices { "4/4", "3/4", "6/8", "7/8", "5/4", "9/8", "12/8", "2/4" };

    static inline const juce::StringArray barsChoices { "4", "8" };

    static inline const juce::StringArray octaveChoices { "-2", "-1", "0", "+1", "+2" };
}#pragma once
