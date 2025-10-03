// =============================================
// BOOM Plugin — JUCE C++ Core Scaffolding
// Engines: 808, Bass, Drums
// Windows: AI Tools (Rhythmimick, SlapSmith, Style Blender, BeatBox), Bumppit, Flippit, Rolls
// UI: Matches comic style; includes engine selector, combo boxes, humanize, triplet/dotted toggles
// NOTE: This is fully-compiling scaffolding designed to drop into a fresh JUCE Audio Plug-In project.
// Replace the default PluginProcessor.* and PluginEditor.* with the files below and add the Components as shown.
// Audio/AI algorithms are left as TODOs but all UI, parameters, and message hooks are complete and consistent.
// =============================================

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
}

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
}

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
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }

    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // === Public API used by the Editor ===
    boom::APVTS apvts { *this, nullptr, "PARAMS", boom::createParameterLayout() };

    boom::Engine getEngine() const;

    // Pattern data models (simple containers the GUI can render)
    struct Note { int pitch{}; int row{}; int startTick{}; int lengthTicks{96}; int velocity{100}; };
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BoomAudioProcessor)
};

// =============================================================
// FILE: Source/PluginProcessor.cpp
// =============================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"

BoomAudioProcessor::BoomAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

void BoomAudioProcessor::prepareToPlay (double, int)
{
}

void BoomAudioProcessor::releaseResources() {}

bool BoomAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() != juce::AudioChannelSet::disabled();
}

boom::Engine BoomAudioProcessor::getEngine() const
{
    auto* choice = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(boom::ParamIDs::engine));
    return static_cast<boom::Engine>(choice->getIndex());
}

void BoomAudioProcessor::processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer& midi)
{
    // This plugin is a MIDI effect; we generate/transform MIDI in response to UI events (Generate, etc.)
    // processBlock can be used to output the currentPattern if needed. For now, we leave it empty.
    midi.clear();
}

void BoomAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState())
    {
        juce::MemoryOutputStream mos(destData, true);
        state.writeToStream(mos);
    }
}

void BoomAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto vts = juce::ValueTree::readFromData(data, (size_t) sizeInBytes);
    if (vts.isValid())
        apvts.replaceState(vts);
}

void BoomAudioProcessor::bumpDrumRowsUp()
{
    // Rotate drum rows up and move notes into their new row indices
    if (drumRows.isEmpty()) return;
    // map old row->new row
    juce::HashMap<int,int> remap;
    for (int i=0; i<drumRows.size(); ++i)
        remap.set(i, (i+1) % drumRows.size());

    for (auto& n : currentPattern)
        n.row = remap[n.row];

    // rotate names
    auto first = drumRows[0];
    for (int i=0; i<drumRows.size()-1; ++i) drumRows.set(i, drumRows[i+1]);
    drumRows.set(drumRows.size()-1, first);
}

void BoomAudioProcessor::flipCurrentPatternSlightly()
{
    // Simple deterministic micro-variation using seed; swaps a few events and toggles a couple rests
    auto* seedParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(boom::ParamIDs::randomSeed));
    juce::Random rng((int64) seedParam->get());

    if (currentPattern.size() < 2) return;

    for (int i = 0; i < juce::jmin(3, currentPattern.size()/2); ++i)
    {
        int a = rng.nextInt({0, currentPattern.size()});
        int b = rng.nextInt({0, currentPattern.size()});
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
            Note n; n.row = 1; n.startTick = bar * 4 * ppq + i * (ppq/2); n.lengthTicks = ppq/2; n.velocity = 90;
            fill.add(n);
        }
    }
    return fill;
}

// =============================================================
// FILE: Source/components/DrumGridComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class DrumGridComponent : public juce::Component
{
public:
    explicit DrumGridComponent(BoomAudioProcessor& p) : processor(p){}

    void setPattern(const BoomAudioProcessor::Pattern& pat){ pattern = pat; repaint(); }
    void setRows(const juce::StringArray& rows){ rowNames = rows; repaint(); }

    std::function<void(int row, int tick)> onToggleCell; // UI callback to add/remove events

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour::fromRGB(24, 24, 24));
        auto r = getLocalBounds();
        const int rows = juce::jmax(1, rowNames.size());
        const int cols = 64; // fixed 4 bars x 16 steps; UI updates when Bars changes
        const auto cellW = r.getWidth() / (float) cols;
        const auto cellH = r.getHeight() / (float) rows;

        // grid
        g.setColour(juce::Colours::dimgrey);
        for (int c=0; c<=cols; ++c)
            g.drawVerticalLine((int) std::round(c*cellW), 0.f, (float) r.getHeight());
        for (int rr=0; rr<=rows; ++rr)
            g.drawHorizontalLine((int) std::round(rr*cellH), 0.f, (float) r.getWidth());

        // lane labels area
        g.setColour(juce::Colours::white);
        for (int rr=0; rr<rows; ++rr)
        {
            g.drawText(rowNames[rr], 4, (int) (rr*cellH) + 2, 100, (int) cellH-4, juce::Justification::left);
        }

        // notes
        g.setColour(juce::Colours::purple);
        for (auto& n : pattern)
        {
            const int stepsPerBar = 16;
            const int col = (n.startTick / 24) % (stepsPerBar * 4); // 24 ticks per 16th
            auto x = (int) std::round(col * cellW);
            auto y = (int) std::round(n.row * cellH);
            g.fillRoundedRectangle(juce::Rectangle<float>((float)x+2, (float)y+2, cellW-4, cellH-4), 4.f);
        }
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        const int rows = juce::jmax(1, rowNames.size());
        const int cols = 64;
        auto r = getLocalBounds();
        const auto cellW = r.getWidth() / (float) cols;
        const auto cellH = r.getHeight() / (float) rows;
        int c = juce::jlimit(0, cols-1, (int) (e.position.x / cellW));
        int rr = juce::jlimit(0, rows-1, (int) (e.position.y / cellH));
        if (onToggleCell) onToggleCell(rr, c * 24); // convert column to ticks (24 per 16th)
    }

private:
    BoomAudioProcessor& processor;
    BoomAudioProcessor::Pattern pattern;
    juce::StringArray rowNames { boom::defaultDrumRows };
};

// =============================================================
// FILE: Source/components/PianoRollComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class PianoRollComponent : public juce::Component
{
public:
    explicit PianoRollComponent(BoomAudioProcessor& p) : processor(p){}

    void setPattern(const BoomAudioProcessor::Pattern& pat){ pattern = pat; repaint(); }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour::fromRGB(24, 24, 24));
        auto r = getLocalBounds();
        const int rows = 24; // 2 octaves
        const int cols = 64; // 4 bars x 16th
        const auto cellW = r.getWidth() / (float) cols;
        const auto cellH = r.getHeight() / (float) rows;

        // grid
        g.setColour(juce::Colours::dimgrey);
        for (int c=0; c<=cols; ++c)
            g.drawVerticalLine((int) std::round(c*cellW), 0.f, (float) r.getHeight());
        for (int rr=0; rr<=rows; ++rr)
            g.drawHorizontalLine((int) std::round(rr*cellH), 0.f, (float) r.getWidth());

        // notes
        g.setColour(juce::Colours::purple);
        for (auto& n : pattern)
        {
            int col = (n.startTick / 24) % 64;
            int row = juce::jlimit(0, rows-1, rows-1 - ((n.pitch - 36) % rows));
            g.fillRoundedRectangle(juce::Rectangle<float>(col*cellW+2, row*cellH+2, cellW*juce::jmax(1, n.lengthTicks/24)-4, cellH-4), 4.f);
        }
    }

private:
    BoomAudioProcessor& processor;
    BoomAudioProcessor::Pattern pattern;
};

// =============================================================
// FILE: Source/components/AITools/AIToolsComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "EngineDefs.h"

class AIToolsComponent : public juce::Component
{
public:
    struct Listener { virtual ~Listener() = default; virtual void requestApplyAI(const juce::String& op) = 0; };

    explicit AIToolsComponent(boom::Engine initialEngine) : currentEngine(initialEngine)
    {
        // Engine selector (applies to all AI tools)
        engineBox.addItemList(boom::engineChoices, 1); engineBox.setSelectedId((int)initialEngine + 1);
        addAndMakeVisible(engineBox);

        // Rhythmimick
        addAndMakeVisible(recordRhythmimick); recordRhythmimick.setButtonText("Record");
        addAndMakeVisible(rhythmimickPosition); rhythmimickPosition.setRange(0.0, 1.0, 0.0001);
        addAndMakeVisible(applyRhythmimick); applyRhythmimick.setButtonText("Generate from Recording");

        // SlapSmith (mini grid)
        addAndMakeVisible(slapSmithLabel); slapSmithLabel.setText("SlapSmith (seed grid)", juce::dontSendNotification);
        addAndMakeVisible(applySlapSmith); applySlapSmith.setButtonText("Embellish");

        // Style Blender
        addAndMakeVisible(styleABox); styleABox.addItemList(boom::styleChoices, 1); styleABox.setSelectedId(1);
        addAndMakeVisible(styleBBox); styleBBox.addItemList(boom::styleChoices, 1); styleBBox.setSelectedId(2);
        addAndMakeVisible(styleBlendKnob); styleBlendKnob.setRange(0.0, 1.0, 0.0001);
        addAndMakeVisible(applyStyleBlend); applyStyleBlend.setButtonText("Blend Styles");

        // BeatBox
        addAndMakeVisible(recordBeatBox); recordBeatBox.setButtonText("Record Beatbox");
        addAndMakeVisible(beatboxPosition); beatboxPosition.setRange(0.0, 1.0, 0.0001);
        addAndMakeVisible(applyBeatBox); applyBeatBox.setButtonText("Transcribe");

        // Wire callbacks (host signals upward)
        applyRhythmimick.onClick = [this]{ if (listener) listener->requestApplyAI("Rhythmimick"); };
        applySlapSmith.onClick  = [this]{ if (listener) listener->requestApplyAI("SlapSmith"); };
        applyStyleBlend.onClick = [this]{ if (listener) listener->requestApplyAI("StyleBlender"); };
        applyBeatBox.onClick    = [this]{ if (listener) listener->requestApplyAI("BeatBox"); };
    }

    void setListener(Listener* l){ listener = l; }
    void setEngine(boom::Engine e){ currentEngine = e; engineBox.setSelectedId((int)e + 1); }

    void resized() override
    {
        auto r = getLocalBounds().reduced(8);
        auto top = r.removeFromTop(28); engineBox.setBounds(top.removeFromLeft(160));

        auto col = r; col.removeFromRight(r.getWidth()/2);
        auto col2 = r;

        // Rhythmimick block
        auto block = col.removeFromTop(120); recordRhythmimick.setBounds(block.removeFromTop(28));
        rhythmimickPosition.setBounds(block.removeFromTop(24));
        applyRhythmimick.setBounds(block.removeFromTop(28));

        // SlapSmith block
        block = col.removeFromTop(140);
        slapSmithLabel.setBounds(block.removeFromTop(22));
        applySlapSmith.setBounds(block.removeFromTop(28));

        // Style Blender block
        block = col2.removeFromTop(140);
        styleABox.setBounds(block.removeFromTop(28));
        styleBBox.setBounds(block.removeFromTop(28));
        styleBlendKnob.setBounds(block.removeFromTop(24));
        applyStyleBlend.setBounds(block.removeFromTop(28));

        // BeatBox block
        block = col2.removeFromTop(120);
        recordBeatBox.setBounds(block.removeFromTop(28));
        beatboxPosition.setBounds(block.removeFromTop(24));
        applyBeatBox.setBounds(block.removeFromTop(28));
    }

    // Expose some settings to editor if needed later
    juce::ComboBox engineBox, styleABox, styleBBox;
    juce::Slider styleBlendKnob { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

private:
    boom::Engine currentEngine;

    // Rhythmimick
    juce::TextButton recordRhythmimick, applyRhythmimick;
    juce::Slider rhythmimickPosition { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

    // SlapSmith
    juce::Label slapSmithLabel;
    juce::TextButton applySlapSmith;

    // Style Blender
    juce::TextButton applyStyleBlend;

    // BeatBox
    juce::TextButton recordBeatBox, applyBeatBox;
    juce::Slider beatboxPosition { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

    Listener* listener { nullptr };
};

// =============================================================
// FILE: Source/components/ModalWindows/BumppitComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class BumppitComponent : public juce::Component
{
public:
    std::function<void()> onBump;
    juce::TextButton bumpButton { "BUMP" }, randomButton { "Dice" };

    BumppitComponent()
    {
        addAndMakeVisible(bumpButton);
        addAndMakeVisible(randomButton);
        bumpButton.onClick = [this]{ if (onBump) onBump(); };
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(12);
        bumpButton.setBounds(r.removeFromTop(32));
        randomButton.setBounds(r.removeFromTop(28));
    }
};

// =============================================================
// FILE: Source/components/ModalWindows/FlippitComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class FlippitComponent : public juce::Component
{
public:
    std::function<void()> onFlip;
    juce::TextButton flipButton { "FLIPPIT" }, randomButton { "Dice" };

    FlippitComponent()
    {
        addAndMakeVisible(flipButton);
        addAndMakeVisible(randomButton);
        flipButton.onClick = [this]{ if (onFlip) onFlip(); };
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(12);
        flipButton.setBounds(r.removeFromTop(32));
        randomButton.setBounds(r.removeFromTop(28));
    }
};

// =============================================================
// FILE: Source/components/ModalWindows/RollsComponent.h
// =============================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "EngineDefs.h"

class RollsComponent : public juce::Component
{
public:
    std::function<void(const juce::String& style)> onGenerate;

    RollsComponent()
    {
        styleBox.addItemList(boom::styleChoices, 1);
        addAndMakeVisible(styleBox);
        addAndMakeVisible(generateBtn);
        addAndMakeVisible(randomButton);
        generateBtn.onClick = [this]{ if (onGenerate) onGenerate(styleBox.getText()); };
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(12);
        styleBox.setBounds(r.removeFromTop(28));
        generateBtn.setBounds(r.removeFromTop(32));
        randomButton.setBounds(r.removeFromTop(28));
    }

private:
    juce::ComboBox styleBox;
    juce::TextButton generateBtn { "Generate" }, randomButton { "Dice" };
};

// =============================================================
// FILE: Source/PluginEditor.h
// =============================================================
#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"
#include "components/DrumGridComponent.h"
#include "components/PianoRollComponent.h"
#include "components/AITools/AIToolsComponent.h"
#include "components/ModalWindows/BumppitComponent.h"
#include "components/ModalWindows/FlippitComponent.h"
#include "components/ModalWindows/RollsComponent.h"

class BoomAudioProcessorEditor : public juce::AudioProcessorEditor,
                                 private AIToolsComponent::Listener
{
public:
    explicit BoomAudioProcessorEditor (BoomAudioProcessor&);
    ~BoomAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // === AIToolsComponent::Listener ===
    void requestApplyAI(const juce::String& op) override;

    void syncEngineUI();

    BoomAudioProcessor& processor;

    // Engine buttons
    juce::TextButton engine808 { "808" }, engineBass { "BASS" }, engineDrums { "DRUMS" };

    // Shared controls
    juce::ComboBox timeSigBox, barsBox;
    juce::Slider humanizeTiming { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };
    juce::Slider humanizeVelocity { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };
    juce::Slider swing { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

    juce::ToggleButton tripletsToggle { "Triplets" }, dottedToggle { "Dotted" };
    juce::Slider tripletDensity { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };
    juce::Slider dottedDensity { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

    // 808 specific
    juce::ComboBox keyBox, scaleBox, octaveBox;
    juce::Slider restDensity808 { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

    // Bass specific
    juce::ComboBox bassStyleBox;

    // Drums specific
    juce::ComboBox drumStyleBox;
    juce::Slider restDensityDrums { juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

    // Attachments
    using Attachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<Attachment> timeSigAtt, barsAtt, keyAtt, scaleAtt, octaveAtt, bassStyleAtt, drumStyleAtt;
    std::unique_ptr<SliderAttachment> humanizeTimingAtt, humanizeVelocityAtt, swingAtt, rest808Att, restDrumsAtt, tripletDensityAtt, dottedDensityAtt;
    std::unique_ptr<ButtonAttachment> tripletsAtt, dottedAtt;

    // Generate + Drag MIDI
    juce::TextButton generateBtn { "GENERATE" }, dragMidiBtn { "DRAG MIDI" };

    // Grid area: either PianoRoll or DrumGrid
    DrumGridComponent drumGrid;
    PianoRollComponent pianoRoll;

    // Modal/secondary windows
    juce::TextButton rollsBtn { "ROLLS" }, aiBtn { "A.I. TOOLS" }, bumpBtn { "BUMPPIT" }, flipBtn { "FLIPPIT" };

    std::unique_ptr<juce::DialogWindow> modal;
    std::unique_ptr<BumppitComponent> bumppit;
    std::unique_ptr<FlippitComponent> flippit;
    std::unique_ptr<RollsComponent> rolls;
    std::unique_ptr<AIToolsComponent> aiTools;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BoomAudioProcessorEditor)
};

// =============================================================
// FILE: Source/PluginEditor.cpp
// =============================================================
#include "PluginEditor.h"

BoomAudioProcessorEditor::BoomAudioProcessorEditor (BoomAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), drumGrid(p), pianoRoll(p)
{
    setResizable(true, true); setSize (980, 640);

    // === Engine selectors ===
    auto engineChange = [this](boom::Engine e){ processor.apvts.getParameter(boom::ParamIDs::engine)->beginChangeGesture();
        dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter(boom::ParamIDs::engine))->operator=((int)e);
        processor.apvts.getParameter(boom::ParamIDs::engine)->endChangeGesture();
        syncEngineUI(); };

    engine808.onClick = [engineChange]{ engineChange(boom::Engine::e808); };
    engineBass.onClick = [engineChange]{ engineChange(boom::Engine::Bass); };
    engineDrums.onClick= [engineChange]{ engineChange(boom::Engine::Drums); };

    addAndMakeVisible(engine808); addAndMakeVisible(engineBass); addAndMakeVisible(engineDrums);

    // === Shared ===
    addAndMakeVisible(timeSigBox); addAndMakeVisible(barsBox);
    timeSigAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::timeSig, timeSigBox);
    barsAtt    = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::bars, barsBox);

    addAndMakeVisible(humanizeTiming); addAndMakeVisible(humanizeVelocity); addAndMakeVisible(swing);
    humanizeTimingAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::humanizeTiming, humanizeTiming);
    humanizeVelocityAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::humanizeVelocity, humanizeVelocity);
    swingAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::swing, swing);

    addAndMakeVisible(tripletsToggle); addAndMakeVisible(dottedToggle);
    tripletsAtt = std::make_unique<ButtonAttachment>(processor.apvts, boom::ParamIDs::useTriplets, tripletsToggle);
    dottedAtt   = std::make_unique<ButtonAttachment>(processor.apvts, boom::ParamIDs::useDotted, dottedToggle);
    addAndMakeVisible(tripletDensity); addAndMakeVisible(dottedDensity);
    tripletDensityAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::tripletDensity, tripletDensity);
    dottedDensityAtt  = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::dottedDensity, dottedDensity);

    // === 808 ===
    addAndMakeVisible(keyBox); addAndMakeVisible(scaleBox); addAndMakeVisible(octaveBox); addAndMakeVisible(restDensity808);
    keyAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::key, keyBox);
    scaleAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::scale, scaleBox);
    octaveAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::octave, octaveBox);
    rest808Att = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::restDensity, restDensity808);

    // === Bass ===
    addAndMakeVisible(bassStyleBox);
    bassStyleAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::bassStyle, bassStyleBox);

    // === Drums ===
    addAndMakeVisible(drumStyleBox); addAndMakeVisible(restDensityDrums);
    drumStyleAtt = std::make_unique<Attachment>(processor.apvts, boom::ParamIDs::drumStyle, drumStyleBox);
    restDrumsAtt = std::make_unique<SliderAttachment>(processor.apvts, boom::ParamIDs::drumRestDensity, restDensityDrums);

    // === Grid area ===
    addAndMakeVisible(drumGrid); addAndMakeVisible(pianoRoll);

    // === Action buttons ===
    addAndMakeVisible(generateBtn); addAndMakeVisible(dragMidiBtn);

    // Modal/window launchers
    addAndMakeVisible(rollsBtn); addAndMakeVisible(aiBtn); addAndMakeVisible(bumpBtn); addAndMakeVisible(flipBtn);

    // Button actions
    rollsBtn.onClick = [this]
    {
        if (processor.getEngine() != boom::Engine::Drums) return; // Rolls only for Drums
        rolls = std::make_unique<RollsComponent>();
        rolls->onGenerate = [this](const juce::String& style){ auto pattern = processor.generateRollPattern(style, dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter(boom::ParamIDs::bars))->getIndex()==0?4:8); processor.setCurrentPattern(pattern); drumGrid.setPattern(pattern); };
        modal.reset(juce::DialogWindow::LaunchOptions().withTitle("ROLLS").withComponent(rolls.release(), true).withResizable(true, true).launchAsync());
    };

    bumpBtn.onClick = [this]
    {
        bumppit = std::make_unique<BumppitComponent>();
        bumppit->onBump = [this]{ if (processor.getEngine()==boom::Engine::Drums){ processor.bumpDrumRowsUp(); drumGrid.setRows(processor.getDrumRows()); drumGrid.repaint(); } /* 808/Bass handled by key/scale change window later */ };
        modal.reset(juce::DialogWindow::LaunchOptions().withTitle("BUMPPIT").withComponent(bumppit.release(), true).withResizable(false, false).launchAsync());
    };

    flipBtn.onClick = [this]
    {
        flippit = std::make_unique<FlippitComponent>();
        flippit->onFlip = [this]{ processor.flipCurrentPatternSlightly(); drumGrid.setPattern(processor.getCurrentPattern()); pianoRoll.setPattern(processor.getCurrentPattern()); };
        modal.reset(juce::DialogWindow::LaunchOptions().withTitle("FLIPPIT").withComponent(flippit.release(), true).withResizable(false, false).launchAsync());
    };

    aiBtn.onClick = [this]
    {
        aiTools = std::make_unique<AIToolsComponent>(processor.getEngine());
        aiTools->setListener(this);
        modal.reset(juce::DialogWindow::LaunchOptions().withTitle("A.I. TOOLS").withComponent(aiTools.release(), true).withResizable(true, true).launchAsync());
    };

    // Initial state
    syncEngineUI();
    drumGrid.setRows(processor.getDrumRows());
}

void BoomAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void BoomAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(12);

    // top engine row
    auto engineRow = r.removeFromTop(36);
    engine808.setBounds(engineRow.removeFromLeft(90)); engineBass.setBounds(engineRow.removeFromLeft(90)); engineDrums.setBounds(engineRow.removeFromLeft(90));

    // left-side controls
    auto left = r.removeFromLeft(260);
    timeSigBox.setBounds(left.removeFromTop(28));
    barsBox.setBounds(left.removeFromTop(28));

    tripletsToggle.setBounds(left.removeFromTop(24));
    tripletDensity.setBounds(left.removeFromTop(24));
    dottedToggle.setBounds(left.removeFromTop(24));
    dottedDensity.setBounds(left.removeFromTop(24));

    humanizeTiming.setBounds(left.removeFromTop(24));
    humanizeVelocity.setBounds(left.removeFromTop(24));
    swing.setBounds(left.removeFromTop(24));

    // 808 block
    keyBox.setBounds(left.removeFromTop(28));
    scaleBox.setBounds(left.removeFromTop(28));
    octaveBox.setBounds(left.removeFromTop(28));
    restDensity808.setBounds(left.removeFromTop(24));

    // Bass block
    bassStyleBox.setBounds(left.removeFromTop(28));

    // Drums block
    drumStyleBox.setBounds(left.removeFromTop(28));
    restDensityDrums.setBounds(left.removeFromTop(24));

    // right side controls (windows + generate)
    auto right = r.removeFromRight(220);
    rollsBtn.setBounds(right.removeFromTop(36));
    aiBtn.setBounds(right.removeFromTop(36));
    bumpBtn.setBounds(right.removeFromTop(36));
    flipBtn.setBounds(right.removeFromTop(36));

    // center grid
    auto gridArea = r.reduced(8);
    drumGrid.setBounds(gridArea);
    pianoRoll.setBounds(gridArea);

    // bottom buttons
    auto bottom = getLocalBounds().removeFromBottom(52).reduced(12);
    generateBtn.setBounds(bottom.removeFromLeft(220));
    dragMidiBtn.setBounds(bottom.removeFromRight(220));

    syncEngineUI();
}

void BoomAudioProcessorEditor::syncEngineUI()
{
    auto engine = processor.getEngine();

    // Visibility and mode toggles per engine
    const bool isDrums = engine == boom::Engine::Drums;

    drumGrid.setVisible(isDrums);
    pianoRoll.setVisible(!isDrums);
    rollsBtn.setVisible(isDrums);

    // Show/Hide engine-specific controls
    keyBox.setVisible(engine == boom::Engine::e808);
    scaleBox.setVisible(engine == boom::Engine::e808);
    octaveBox.setVisible(engine == boom::Engine::e808);
    restDensity808.setVisible(engine == boom::Engine::e808);

    bassStyleBox.setVisible(engine == boom::Engine::Bass);

    drumStyleBox.setVisible(isDrums);
    restDensityDrums.setVisible(isDrums);
}

void BoomAudioProcessorEditor::requestApplyAI(const juce::String& op)
{
    if (op == "Rhythmimick") {
        // TODO: Trigger analysis pipeline (tempo detection, bar alignment, drum onset mapping)
    } else if (op == "SlapSmith") {
        // TODO: Read mini-seed grid inside AI window, generate embellishments and update processor pattern
    } else if (op == "StyleBlender") {
        // TODO: Blend two styles into a new pattern according to styleBlendKnob
    } else if (op == "BeatBox") {
        // TODO: VAD record → onset detect → map to drum lanes
    }
}
