#include "common.hpp"
#include "data/internal_patch.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace StreichfettSse
{
namespace InternalPatch
{

// public
bool current_patch_changed = false;

// private
SoundAddress _current_sound;
SoundModel::Patch _original_patch("original");
SoundModel::Patch _current_patch("current");

void initData() noexcept
{
    _current_sound.sound = 0;
    _original_patch.init();
    _current_patch.init();
    current_patch_changed = false;
}

SoundAddress* getCurrentSoundAddress() noexcept
{
    return &_current_sound;
}

void setCurrentSoundAddress(const int sound) noexcept
{
    _current_sound.sound = sound;
}

// DSI: Streichfett
char getSoundBankChar(const int sound) noexcept
{
    return 'A' + sound / 4;
}

// DSI: Streichfett
int getSoundPatchNumber(const int sound) noexcept
{
    return sound % 4 + 1;
}

SoundModel::Patch* getOriginalPatch() noexcept
{
    return &_original_patch;
}

SoundModel::Patch* getCurrentPatch() noexcept
{
    return &_current_patch;
}

// DSI: Streichfett
void setPatchFromBytes(SoundModel::Patch* patch, const ByteVec& data)
{
    using namespace SoundModel;

    current_patch_changed = false;

    // convert Device values to Editor values
    try
    {
        patch->registration     = data[(int)ParamIndex::Registration];
        patch->octave_switch    = data[(int)ParamIndex::OctaveSwitch];
        patch->ensemble_effect  = data[(int)ParamIndex::EnsembleEffect];
        patch->ensemble         = data[(int)ParamIndex::Ensemble];
        patch->crescendo        = data[(int)ParamIndex::Crescendo];
        patch->release          = data[(int)ParamIndex::Release];
        patch->tone             = data[(int)ParamIndex::Tone];
        patch->tremolo          = data[(int)ParamIndex::Tremolo];
        patch->split_layer      = data[(int)ParamIndex::SplitLayer];
        patch->split_key        = data[(int)ParamIndex::SplitKey];
        patch->envelope_mode    = data[(int)ParamIndex::EnvelopeMode];
        patch->attack           = data[(int)ParamIndex::Attack];
        patch->decay            = data[(int)ParamIndex::Decay];
        patch->balance          = data[(int)ParamIndex::Balance];
        patch->animate          = data[(int)ParamIndex::Animate];
        patch->phaser           = data[(int)ParamIndex::Phaser];
        patch->reverb           = data[(int)ParamIndex::Reverb];
    }
    catch (std::exception& error)
    {
        // The received tone data seems to be invalid
        throw std::exception(StringUtil::format("setPatchFromBytes failed: %s", error.what()).c_str());
    }
}

// DSI: Streichfett
ByteVec getDataBytes(const SoundModel::Patch* const patch)
{
    ByteVec data;

    // convert Editor values to Device values
    data.clear();
    data.push_back(patch->registration.toDv());
    data.push_back(patch->octave_switch.toDv());
    data.push_back(patch->ensemble_effect.toDv());
    data.push_back(patch->ensemble.toDv());
    data.push_back(patch->crescendo.toDv());
    data.push_back(patch->release.toDv());
    data.push_back(patch->tone.toDv());
    data.push_back(patch->tremolo.toDv());
    data.push_back(patch->split_layer.toDv());
    data.push_back(patch->split_key.toDv());
    data.push_back(patch->envelope_mode.toDv());
    data.push_back(patch->attack.toDv());
    data.push_back(patch->decay.toDv());
    data.push_back(patch->balance.toDv());
    data.push_back(patch->animate.toDv());
    data.push_back(patch->phaser.toDv());
    data.push_back(patch->reverb.toDv());
    data.push_back(0);      // Reserved
    data.push_back(0);      // Reserved
    data.push_back(0);      // Reserved
    data.push_back(0);      // Reserved
    data.push_back(0);      // Reserved
    data.push_back(0);      // Reserved
    data.push_back(0);      // Reserved

    return data;
}

// DSI: Streichfett
void copyPatchAtoB(SoundModel::Patch* a, SoundModel::Patch* b)
{
    b->registration = a->registration;
    b->octave_switch = a->octave_switch;
    b->ensemble_effect = a->ensemble_effect;
    b->ensemble = a->ensemble;
    b->crescendo = a->crescendo;
    b->release = a->release;
    b->tone = a->tone;
    b->tremolo = a->tremolo;
    b->split_layer = a->split_layer;
    b->split_key = a->split_key;
    b->envelope_mode = a->envelope_mode;
    b->attack = a->attack;
    b->decay = a->decay;
    b->balance = a->balance;
    b->animate = a->animate;
    b->phaser = a->phaser;
    b->reverb = a->reverb;
}

} // InternalPatch
} // StreichfettSse
