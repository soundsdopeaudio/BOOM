
// =============================================================
// FILE: Source/PluginProcessor.h
// =============================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "ProcessorParameters.h"
#include "EngineDefs.h"

class BoomAudioProcessor : public juce::AudioProcessor
{
public:
    BoomAudioProcessor();
    ~BoomAudioProcessor() override = default;

    // AudioProcessor overrides
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }

    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // === Public API used by the Editor ===
    boom::APVTS apvts { *this, nullptr, "PARAMS", boom::createParameterLayout() };

    boom::Engine getEngine() const;

    // Pattern data models (simple containers the GUI can render)
    struct Note { int pitch{}; int row{}; int startTick{}; int lengthTicks{ 96 }; int velocity{ 100 }; };
    using Pattern = juce::Array<Note>;

    const Pattern& getCurrentPattern() const { return currentPattern; }
    void setCurrentPattern(const Pattern& p) { currentPattern = p; }

    // Drums grid uses rows instead of MIDI pitch names
    const juce::StringArray& getDrumRows() const { return drumRows; }
    void bumpDrumRowsUp(); // Bumppit for Drums

    // Flip (small variation)
    void flipCurrentPatternSlightly();

    // Rolls generator (returns a short fill pattern)
    Pattern generateRollPattern(const juce::String& style, int bars);

private:
    Pattern currentPattern;
    juce::StringArray drumRows { boom::defaultDrumRows };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoomAudioProcessor)
}; #pragma once
