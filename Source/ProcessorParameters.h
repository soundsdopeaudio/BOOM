// =============================================================
// FILE: Source/ProcessorParameters.h
// =============================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "EngineDefs.h"

namespace boom
{
    using APVTS = juce::AudioProcessorValueTreeState;

    inline juce::String paramId(const juce::String& s) { return s; }

    struct ParamIDs
    {
        // Global
        static constexpr const char* engine = "engine"; // choice: 808, Bass, Drums
        static constexpr const char* timeSig = "timeSig"; // choice
        static constexpr const char* bars = "bars";       // choice 4 or 8

        // Humanize + feel
        static constexpr const char* humanizeTiming = "humanizeTiming"; // ms
        static constexpr const char* humanizeVelocity = "humanizeVelocity"; // 0..1
        static constexpr const char* swing = "swing"; // 0..1

        // Triplets / Dotted
        static constexpr const char* useTriplets = "useTriplets"; // bool
        static constexpr const char* tripletDensity = "tripletDensity"; // 0..1
        static constexpr const char* useDotted = "useDotted"; // bool
        static constexpr const char* dottedDensity = "dottedDensity"; // 0..1

        // 808 specific
        static constexpr const char* key = "key"; // choice
        static constexpr const char* scale = "scale"; // choice
        static constexpr const char* octave = "octave"; // choice
        static constexpr const char* restDensity = "restDensity"; // 0..1

        // Bass specific
        static constexpr const char* bassStyle = "bassStyle"; // choice (styleChoices)

        // Drums specific
        static constexpr const char* drumStyle = "drumStyle"; // choice (styleChoices)
        static constexpr const char* drumRestDensity = "drumRestDensity"; // 0..1

        // Seeds
        static constexpr const char* randomSeed = "randomSeed"; // int
    };

    inline APVTS::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

        // Global
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::engine, "Engine", engineChoices, (int)Engine::Drums));
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::timeSig, "Time Signature", timeSigChoices, 0));
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::bars, "Bars", barsChoices, 0));

        // Humanize + swing
        p.emplace_back(std::make_unique<juce::AudioParameterFloat>(ParamIDs::humanizeTiming, "Humanize Timing (ms)", juce::NormalisableRange<float>(0.f, 25.f, 0.01f), 0.f));
        p.emplace_back(std::make_unique<juce::AudioParameterFloat>(ParamIDs::humanizeVelocity, "Humanize Velocity", juce::NormalisableRange<float>(0.f, 1.f, 0.0001f), 0.f));
        p.emplace_back(std::make_unique<juce::AudioParameterFloat>(ParamIDs::swing, "Swing", juce::NormalisableRange<float>(0.f, 1.f, 0.0001f), 0.f));

        // Triplet/Dotted
        p.emplace_back(std::make_unique<juce::AudioParameterBool>(ParamIDs::useTriplets, "Triplets", false));
        p.emplace_back(std::make_unique<juce::AudioParameterFloat>(ParamIDs::tripletDensity, "Triplet Density", juce::NormalisableRange<float>(0.f, 1.f, 0.0001f), 0.f));
        p.emplace_back(std::make_unique<juce::AudioParameterBool>(ParamIDs::useDotted, "Dotted", false));
        p.emplace_back(std::make_unique<juce::AudioParameterFloat>(ParamIDs::dottedDensity, "Dotted Density", juce::NormalisableRange<float>(0.f, 1.f, 0.0001f), 0.f));

        // 808
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::key, "Key", keyChoices, 0));
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::scale, "Scale", scaleChoices, 0));
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::octave, "Octave", octaveChoices, 2));
        p.emplace_back(std::make_unique<juce::AudioParameterFloat>(ParamIDs::restDensity, "Rest Density (808)", juce::NormalisableRange<float>(0.f, 1.f, 0.0001f), 0.0f));

        // Bass
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::bassStyle, "Bass Style", styleChoices, 0));

        // Drums
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::drumStyle, "Drum Style", styleChoices, 0));
        p.emplace_back(std::make_unique<juce::AudioParameterFloat>(ParamIDs::drumRestDensity, "Rest Density (Drums)", juce::NormalisableRange<float>(0.f, 1.f, 0.0001f), 0.0f));

        // Seed
        p.emplace_back(std::make_unique<juce::AudioParameterInt>(ParamIDs::randomSeed, "Seed", 0, 1'000'000, 0));

        return { p.begin(), p.end() };
    }
}#pragma once
