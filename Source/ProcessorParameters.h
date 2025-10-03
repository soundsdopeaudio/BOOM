#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "EngineDefs.h"

namespace boom
{
    using APVTS = juce::AudioProcessorValueTreeState;

    juce::String paramId(const juce::String& s);

    struct ParamIDs
    {
        // Global
        static constexpr const char* engine = "engine";
        static constexpr const char* timeSig = "timeSig";
        static constexpr const char* bars = "bars";

        // Humanize + feel
        static constexpr const char* humanizeTiming = "humanizeTiming";
        static constexpr const char* humanizeVelocity = "humanizeVelocity";
        static constexpr const char* swing = "swing";

        // Triplets / Dotted
        static constexpr const char* useTriplets = "useTriplets";
        static constexpr const char* tripletDensity = "tripletDensity";
        static constexpr const char* useDotted = "useDotted";
        static constexpr const char* dottedDensity = "dottedDensity";

        // 808 specific
        static constexpr const char* key = "key";
        static constexpr const char* scale = "scale";
        static constexpr const char* octave = "octave";
        static constexpr const char* restDensity = "restDensity";

        // Bass specific
        static constexpr const char* bassStyle = "bassStyle";

        // Drums specific
        static constexpr const char* drumStyle = "drumStyle";
        static constexpr const char* drumRestDensity = "drumRestDensity";

        // Seeds
        static constexpr const char* randomSeed = "randomSeed";
    };

    APVTS::ParameterLayout createParameterLayout();
}