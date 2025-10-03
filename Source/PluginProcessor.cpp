// =============================================================
// FILE: Source/PluginProcessor.cpp
// =============================================================
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
        // clear outgoing MIDI buffer before we add scheduled events
        midi.clear();

        // --- Get the host playhead and current transport/tempo/PPQ info ---
        auto* playhead = getPlayHead();
        if (playhead == nullptr)
            return; // no host info available

        if (!playhead->getCurrentPosition(playheadPos))
        {
            // couldn't obtain position info this block
            return;
        }

        // If transport isn't playing, don't output pattern MIDI
        if (!playheadPos.isPlaying)
            return;

        // ---- Pattern / timing configuration ----
        // Number of bars for the internal pattern (4 or 8 from APVTS)
        const int numBars = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(boom::ParamIDs::bars))->getIndex() == 0 ? 4 : 8;

        // Internal resolution (ticks per quarter note)
        const double ticksPerQuarter = 96.0; // matches your generateRollPattern / note tick math

        // Beats per bar = numerator * (4 / denominator)
        const double beatsPerBar = (double)playheadPos.timeSigNumerator * (4.0 / (double)playheadPos.timeSigDenominator);
        const double ticksPerBar = ticksPerQuarter * beatsPerBar;
        const double patternLengthTicks = (double)numBars * ticksPerBar;

        // Use ppqPosition (beats since start) for an accurate beat position from the host
        // Convert beats -> ticks:
        const double currentPosTicks = playheadPos.ppqPosition * ticksPerQuarter;

        // Compute where we are inside our looping pattern (wrap to pattern length)
        double loopPosTicks = 0.0;
        if (patternLengthTicks > 0.0)
            loopPosTicks = std::fmod(currentPosTicks, patternLengthTicks);

        // Compute ticks -> seconds conversion for the current tempo:
        // ticksPerSecond = (BPM / 60) * ticksPerQuarter
        const double ticksPerSecond = (playheadPos.bpm / 60.0) * ticksPerQuarter;

        // Block timing
        const double sampleRate = getSampleRate();
        const int samplesInBlock = getBlockSize();
        const double blockDurationSeconds = (samplesInBlock > 0 && sampleRate > 0.0) ? (double)samplesInBlock / sampleRate : 0.0;

        // Start/end ticks for this audio block (relative to the loop pattern)
        const double blockStartTicks = loopPosTicks;
        const double blockEndTicks = loopPosTicks + blockDurationSeconds * ticksPerSecond;

        // Helper: convert a tick position to a sample offset inside this block:
        auto tickToSamplePos = [&](double tickPos) -> int
        {
            // delta ticks relative to block start
            double dticks = tickPos - blockStartTicks;
            // convert ticks -> seconds -> samples
            double seconds = (ticksPerSecond > 0.0) ? (dticks / ticksPerSecond) : 0.0;
            int sample = (int)std::round(seconds * sampleRate);
            return juce::jlimit(0, samplesInBlock - 1, sample);
        };

        // ---- Walk all notes and add NoteOn/NoteOff events that fall within this block ----
        for (const auto& note : currentPattern)
        {
            const double noteStartTicks = (double)note.startTick;
            const double noteEndTicks = noteStartTicks + (double)note.lengthTicks;

            // map row->MIDI pitch for drums, or use note.pitch for melodic engines
            int pitch = (getEngine() == boom::Engine::Drums) ? (35 + note.row) : note.pitch;

            // Because the pattern is looped, a note may occur either in the base cycle or wrapped around
            // We'll check the note in two cycles (current + next) to catch events near the boundary
            for (int loop = 0; loop < 2; ++loop)
            {
                double loopOffset = (double)loop * patternLengthTicks;
                double sTick = noteStartTicks + loopOffset;
                double eTick = noteEndTicks + loopOffset;

                // NoteOn inside this block?
                if (sTick >= blockStartTicks && sTick < blockEndTicks)
                {
                    int samplePos = tickToSamplePos(sTick);
                    midi.addEvent(juce::MidiMessage::noteOn(1, pitch, (juce::uint8)note.velocity), samplePos);
                }

                // NoteOff inside this block?
                if (eTick >= blockStartTicks && eTick < blockEndTicks)
                {
                    int samplePos = tickToSamplePos(eTick);
                    midi.addEvent(juce::MidiMessage::noteOff(1, pitch), samplePos);
                }
            }
        }

        // (Optional) TODO: you can add logic here to react to tempo changes, e.g.
        // keep a cached tempo value and trigger an event when playheadPos.bpm changes.
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
    juce::Random rng(int64(seedParam->get()));

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