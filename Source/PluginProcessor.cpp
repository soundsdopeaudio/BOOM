#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

#ifndef JucePlugin_Name
#define JucePlugin_Name "BOOM"
#endif

BoomAudioProcessor::BoomAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "PARAMS", boom::createParameterLayout())
    , drumRows(boom::getDefaultDrumRows())
{
}

const juce::String BoomAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

void BoomAudioProcessor::prepareToPlay(double, int)
{
}

void BoomAudioProcessor::releaseResources() {}

bool BoomAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() != juce::AudioChannelSet::disabled();
}

boom::Engine BoomAudioProcessor::getEngine() const
{
    auto* choice = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(boom::ParamIDs::engine));
    return static_cast<boom::Engine>(choice->getIndex());
}

void BoomAudioProcessor::processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer & midi)
    {
        midi.clear();

        auto* playhead = getPlayHead();
        if (playhead == nullptr)
            return;

        auto positionInfo = playhead->getPosition();
        if (!positionInfo)
            return;

        if (!positionInfo->getIsPlaying())
            return;

        const int numBars = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(boom::ParamIDs::bars))->getIndex() == 0 ? 4 : 8;

        const double ticksPerQuarter = 96.0;
        
        const auto timeSignature = positionInfo->getTimeSignature().orFallback ({ 4, 4 });
        const double beatsPerBar = (double)timeSignature.numerator * (4.0 / (double)timeSignature.denominator);
        const double ticksPerBar = ticksPerQuarter * beatsPerBar;
        const double patternLengthTicks = (double)numBars * ticksPerBar;

        const double currentPosTicks = positionInfo->getPpqPosition().orFallback(0.0) * ticksPerQuarter;

        double loopPosTicks = 0.0;
        if (patternLengthTicks > 0.0)
            loopPosTicks = std::fmod(currentPosTicks, patternLengthTicks);

        const double bpm = positionInfo->getBpm().orFallback(120.0);
        const double ticksPerSecond = (bpm / 60.0) * ticksPerQuarter;

        const double sampleRate = getSampleRate();
        const int samplesInBlock = getBlockSize();
        const double blockDurationSeconds = (samplesInBlock > 0 && sampleRate > 0.0) ? (double)samplesInBlock / sampleRate : 0.0;

        const double blockStartTicks = loopPosTicks;
        const double blockEndTicks = loopPosTicks + blockDurationSeconds * ticksPerSecond;

        auto tickToSamplePos = [&](double tickPos) -> int
        {
            double dticks = tickPos - blockStartTicks;
            double seconds = (ticksPerSecond > 0.0) ? (dticks / ticksPerSecond) : 0.0;
            int sample = (int)std::round(seconds * sampleRate);
            return juce::jlimit(0, samplesInBlock - 1, sample);
        };

        for (const auto& note : currentPattern)
        {
            const double noteStartTicks = (double)note.startTick;
            const double noteEndTicks = noteStartTicks + (double)note.lengthTicks;

            int pitch = (getEngine() == boom::Engine::Drums) ? (35 + note.row) : note.pitch;

            for (int loop = 0; loop < 2; ++loop)
            {
                double loopOffset = (double)loop * patternLengthTicks;
                double sTick = noteStartTicks + loopOffset;
                double eTick = noteEndTicks + loopOffset;

                if (sTick >= blockStartTicks && sTick < blockEndTicks)
                {
                    int samplePos = tickToSamplePos(sTick);
                    midi.addEvent(juce::MidiMessage::noteOn(1, pitch, (juce::uint8)note.velocity), samplePos);
                }

                if (eTick >= blockStartTicks && eTick < blockEndTicks)
                {
                    int samplePos = tickToSamplePos(eTick);
                    midi.addEvent(juce::MidiMessage::noteOff(1, pitch), samplePos);
                }
            }
        }
    }

void BoomAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
    {
        juce::MemoryOutputStream mos(destData, true);
        state.writeToStream(mos);
    }
}

void BoomAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto vts = juce::ValueTree::readFromData(data, (size_t)sizeInBytes);
    if (vts.isValid())
        apvts.replaceState(vts);
}

void BoomAudioProcessor::bumpDrumRowsUp()
{
    if (drumRows.isEmpty()) return;
    juce::HashMap<int, int> remap;
    for (int i = 0; i < drumRows.size(); ++i)
        remap.set(i, (i + 1) % drumRows.size());

    for (auto& n : currentPattern)
        n.row = remap[n.row];

    auto first = drumRows[0];
    for (int i = 0; i < drumRows.size() - 1; ++i) drumRows.set(i, drumRows[i + 1]);
    drumRows.set(drumRows.size() - 1, first);
}

void BoomAudioProcessor::flipCurrentPatternSlightly()
{
    auto* seedParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(boom::ParamIDs::randomSeed));
    juce::Random rng((juce::int64)seedParam->get());

    if (currentPattern.size() < 2) return;

    for (int i = 0; i < juce::jmin(3, currentPattern.size() / 2); ++i)
    {
        int a = rng.nextInt({ 0, currentPattern.size() });
        int b = rng.nextInt({ 0, currentPattern.size() });
        std::swap(currentPattern.getReference(a).startTick, currentPattern.getReference(b).startTick);
    }
}

BoomAudioProcessor::Pattern BoomAudioProcessor::generateRollPattern(const juce::String& style, int bars)
{
    juce::ignoreUnused(style);
    Pattern fill;
    const int ppq = 96;
    for (int bar = 0; bar < bars; ++bar)
    {
        for (int i = 0; i < 8; ++i)
        {
            Note n; n.row = 1; n.startTick = bar * 4 * ppq + i * (ppq / 2); n.lengthTicks = ppq / 2; n.velocity = 90;
            fill.add(n);
        }
    }
    return fill;
}

juce::AudioProcessorEditor* BoomAudioProcessor::createEditor()
{
    return new BoomAudioProcessorEditor(*this);
}