#pragma once
#include "model/ev.hpp"
#include "model/sound_const.hpp"
#include "model/sound_value_util.hpp"

namespace StreichfettSse
{
namespace SoundModel
{

// DSI: Streichfett
constexpr auto PNAME_REGISTRATION = "Registration";
constexpr auto PNAME_OCTAVE_SWITCH = "Octave Switch";
constexpr auto PNAME_ENSEMBLE_EFFECT = "Ensemble Effect";
constexpr auto PNAME_ENSEMBLE = "Ensemble";
constexpr auto PNAME_CRESCENDO = "Crescendo";
constexpr auto PNAME_RELEASE = "Release";
constexpr auto PNAME_TONE = "Tone";
constexpr auto PNAME_TREMOLO = "Tremolo";
constexpr auto PNAME_SPLIT_LAYER = "Split/Layer";
constexpr auto PNAME_SPLIT_KEY = "Split key";
constexpr auto PNAME_ENVELOPE_MODE = "Envelope Mode";
constexpr auto PNAME_ATTACK = "Attack";
constexpr auto PNAME_DECAY = "Decay";
constexpr auto PNAME_BALANCE = "Balance";
constexpr auto PNAME_ANIMATE = "Animate";
constexpr auto PNAME_PHASER = "Phaser";
constexpr auto PNAME_REVERB = "Reverb";

// DSI: Streichfett
enum class ParamIndex : int
{
    Registration = 0,
    OctaveSwitch,
    EnsembleEffect,
    Ensemble,
    Crescendo,
    Release,
    Tone,
    Tremolo,
    SplitLayer,
    SplitKey,
    EnvelopeMode,
    Attack,
    Decay,
    Balance,
    Animate,
    Phaser,
    Reverb,
};

// DSI: Streichfett
const std::unordered_map<ParamIndex, int> PARAM_CC
{
                                        // Control Name
    {ParamIndex::Registration, 70},     // String Registration
    {ParamIndex::OctaveSwitch, 71},     // String Octaves
    {ParamIndex::EnsembleEffect, 74},   // String Ensemble Type
    {ParamIndex::Ensemble, 75},         // String Ensemble
    {ParamIndex::Crescendo, 73},        // Crescendo
    {ParamIndex::Release, 72},          // String Release
    {ParamIndex::Tone, 76},             // Solo Tone
    {ParamIndex::Tremolo, 77},          // Solo Tremolo
    {ParamIndex::SplitLayer, 78},       // Solo Split
    {ParamIndex::EnvelopeMode, 79},     // Solo Sustain
    {ParamIndex::Attack, 80},           // Solo Attack
    {ParamIndex::Decay, 81},            // Solo Decay
    {ParamIndex::Balance, 82},          // Balance
    {ParamIndex::Animate, 92},          // FX Animate amount
    {ParamIndex::Phaser, 93},           // FX Phaser amount
    {ParamIndex::Reverb, 94},           // FX Reverb amount
};

// DSI: Streichfett
struct Patch
{
    std::string patch_name;

    Ev registration{ PNAME_REGISTRATION, 0, 127, 0 };

    // 0 - 2    ref EV_OCTAVE_SWITCH
    Ev octave_switch{ PNAME_OCTAVE_SWITCH, 0, 2, 0, &EV_OCTAVE_SWITCH };

    // 0 - 2    ref EV_ENSEMBLE_EFFECT
    Ev ensemble_effect{ PNAME_ENSEMBLE_EFFECT, 0, 2, 1, &EV_ENSEMBLE_EFFECT };

    Ev ensemble{ PNAME_ENSEMBLE, 0, 1, 1, &EV_BINARY };

    Ev crescendo{ PNAME_CRESCENDO, 0, 127, 32 };

    Ev release{ PNAME_RELEASE, 0, 127, 64 };

    Ev tone{ PNAME_TONE, 0, 127, 0 };

    Ev tremolo{ PNAME_TREMOLO, 0, 127, 0 };

    // 0 - 2    ref EV_SPLIT_LAYER
    Ev split_layer{ PNAME_SPLIT_LAYER, 0, 2, 1, &EV_SPLIT_LAYER };

    Ev split_key{ PNAME_SPLIT_KEY, 0, 127, 60, &EV_SPLIT_KEY };

    // 0 - 1    ref EV_ENVELOPE_MODE
    Ev envelope_mode{ PNAME_ENVELOPE_MODE, 0, 1, 1, &EV_ENVELOPE_MODE };

    Ev attack{ PNAME_ATTACK, 0, 127, 0 };

    Ev decay{ PNAME_DECAY, 0, 127, 64 };

    //          ev = value - 64
    Ev balance{ PNAME_BALANCE, -64, 63, 0, nullptr, toCenterEv, centerEvToDv };

    Ev animate{ PNAME_ANIMATE, 0, 127, 0 };

    Ev phaser{ PNAME_PHASER, 0, 127, 0 };

    Ev reverb{ PNAME_REVERB, 0, 127, 0 };

    explicit Patch()
        : patch_name("")
    {
        init();
    }

    explicit Patch(const char* name)
        : patch_name(name)
    {
        init();
    }

    void init() noexcept
    {
        registration.setDefault();
        octave_switch.setDefault();
        ensemble_effect.setDefault();
        ensemble.setDefault();
        crescendo.setDefault();
        release.setDefault();
        tone.setDefault();
        tremolo.setDefault();
        split_layer.setDefault();
        split_key.setDefault();
        envelope_mode.setDefault();
        attack.setDefault();
        decay.setDefault();
        balance.setDefault();
        animate.setDefault();
        phaser.setDefault();
        reverb.setDefault();
    }
};

} // SoundModel
} // StreichfettSse
