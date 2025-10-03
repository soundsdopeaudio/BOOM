// =============================================================
// FILE: Source/PluginProcessor.cpp
// =============================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

BoomAudioProcessor::BoomAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
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

void BoomAudioProcessor::processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer& midi)
{
    midi.clear();

    // GET PLAYHEAD
    auto playhead = getPlayHead();
    if (!playhead || !playhead->getCurrentPosition(playheadPos))
    {
        // Not playing or no transport info, so do nothing.
        return;
    }

    if (!playheadPos.getIsPlaying())
        return;

    // From APVTS, we know how many bars our pattern is.
    auto numBars = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(boom::ParamIDs::bars))->getIndex() == 0 ? 4 : 8;

    // Our internal resolution is 96 PPQ (see generateRollPattern)
    const double ticksPerBeat = 96.0;
    const double beatsPerBar = *playheadPos.getTimeSignature()->numerator * (4.0 / *playheadPos.getTimeSignature()->denominator);
    const double ticksPerBar = ticksPerBeat * beatsPerBar;
    const double patternLengthTicks = numBars * ticksPerBar;

    // Current position in ticks
    double currentPosBeats = *playheadPos.getBpm() / 60.0 * *playheadPos.getTimeInSeconds();
    double currentPosTicks = currentPosBeats * ticksPerBeat;

    // Loop position
    double loopPosTicks = std::fmod(currentPosTicks, patternLengthTicks);

    // Block range in ticks
    double blockRate = *playheadPos.getBpm() / 60.0 * ticksPerBeat;
    double blockStartTicks = loopPosTicks;
    double blockEndTicks = loopPosTicks + getBlockSize() / getSampleRate() * blockRate;


    for (const auto& note : currentPattern)
    {
        double noteStart = (double)note.startTick;
        double noteEnd = noteStart + note.lengthTicks;

        // MIDI pitch: use row for drums, pitch for melodic
        int pitch = (getEngine() == boom::Engine::Drums) ? 35 + note.row : note.pitch;


        // Handle looping for notes that span the pattern boundary
        for (int loop = 0; loop < 2; ++loop)
        {
            double loopOffset = loop * patternLengthTicks;

            // Note On
            if (noteStart + loopOffset >= blockStartTicks && noteStart + loopOffset < blockEndTicks)
            {
                int samplePos = (int)std::round((noteStart + loopOffset - blockStartTicks) / blockRate * getSampleRate());
                midi.addEvent(juce::MidiMessage::noteOn(1, pitch, (juce::uint8)note.velocity), samplePos);
            }

            // Note Off
            if (noteEnd + loopOffset >= blockStartTicks && noteEnd + loopOffset < blockEndTicks)
            {
                int samplePos = (int)std::round((noteEnd + loopOffset - blockStartTicks) / blockRate * getSampleRate());
                midi.addEvent(juce::MidiMessage::noteOff(1, pitch), samplePos);
            }
        }
    }
}

void BoomAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState())
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
    // Rotate drum rows up and move notes into their new row indices
    if (drumRows.isEmpty()) return;
    // map old row->new row
    juce::HashMap<int, int> remap;
    for (int i = 0; i < drumRows.size(); ++i)
        remap.set(i, (i + 1) % drumRows.size());

    for (auto& n : currentPattern)
        n.row = remap[n.row];

    // rotate names
    auto first = drumRows[0];
    for (int i = 0; i < drumRows.size() - 1; ++i) drumRows.set(i, drumRows[i + 1]);
    drumRows.set(drumRows.size() - 1, first);
}

void BoomAudioProcessor::flipCurrentPatternSlightly()
{
    // Simple deterministic micro-variation using seed; swaps a few events and toggles a couple rests
    auto* seedParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(boom::ParamIDs::randomSeed));
    juce::Random rng((int64)seedParam->get());

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
    // Very basic 1-bar snare roll on row 1 (Snare/Clap) repeated across bars
    const int ppq = 96; // 1/4 note grid (24 = 16th)
    for (int bar = 0; bar < bars; ++bar)
    {
        for (int i = 0; i < 8; ++i) // 8th-note subdivisions
        {
            Note n; n.row = 1; n.startTick = bar * 4 * ppq + i * (ppq / 2); n.lengthTicks = ppq / 2; n.velocity = 90;
            fill.add(n);
        }
    }
    return fill;
}