#include "ProcessorParameters.h"

namespace boom
{
    juce::String paramId(const juce::String& s) { return s; }

    APVTS::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

        // Global
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::engine, "Engine", boom::getEngineChoices(), (int)Engine::Drums));
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::timeSig, "Time Signature", boom::getTimeSigChoices(), 0));
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::bars, "Bars", boom::getBarsChoices(), 0));

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
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::key, "Key", boom::getKeyChoices(), 0));
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::scale, "Scale", boom::getScaleChoices(), 0));
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::octave, "Octave", boom::getOctaveChoices(), 2));
        p.emplace_back(std::make_unique<juce::AudioParameterFloat>(ParamIDs::restDensity, "Rest Density (808)", juce::NormalisableRange<float>(0.f, 1.f, 0.0001f), 0.0f));

        // Bass
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::bassStyle, "Bass Style", boom::getStyleChoices(), 0));

        // Drums
        p.emplace_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::drumStyle, "Drum Style", boom::getStyleChoices(), 0));
        p.emplace_back(std::make_unique<juce::AudioParameterFloat>(ParamIDs::drumRestDensity, "Rest Density (Drums)", juce::NormalisableRange<float>(0.f, 1.f, 0.0001f), 0.0f));

        // Seed
        p.emplace_back(std::make_unique<juce::AudioParameterInt>(ParamIDs::randomSeed, "Seed", 0, 1000000, 0));

        return { p.begin(), p.end() };
    }
}