#include "common.hpp"
#include "data/internal_patch.hpp"

namespace StreichfettSse
{
namespace InternalPatch
{

// public
bool current_patch_changed;

// private
PatchAddress current_patch_addr_;
SoundModel::Patch original_patch_("original");
SoundModel::Patch current_patch_("current");

// DSI: Streichfett
void initData() noexcept
{
    current_patch_addr_.sound = 0;
    original_patch_.init();
    current_patch_.init();
    current_patch_changed = false;
}

const PatchAddress& getCurrentPatchAddress() noexcept
{
    return current_patch_addr_;
}

// DSI: Streichfett
void setCurrentPatchAddress(int sound) noexcept
{
    current_patch_addr_.sound = sound;
}

// DSI: Streichfett
char getPatchBankChar(int sound) noexcept
{
    return 'A' + sound / 4;
}

// DSI: Streichfett
char getPatchBankChar(const PatchAddress& patch_addr) noexcept
{
    return getPatchBankChar(patch_addr.sound);
}

// DSI: Streichfett
int getPatchSoundNumber(int sound) noexcept
{
    return sound % 4 + 1;
}

// DSI: Streichfett
int getPatchSoundNumber(const PatchAddress& patch_addr) noexcept
{
    return getPatchSoundNumber(patch_addr.sound);
}

SoundModel::Patch& getOriginalPatch() noexcept
{
    return original_patch_;
}

SoundModel::Patch& getCurrentPatch() noexcept
{
    return current_patch_;
}

// DSI: Streichfett
void setPatchFromBytes(SoundModel::Patch& patch, const ByteVec& data)
{
    using namespace SoundModel;

    current_patch_changed = false;

    // convert Device values to Editor values
    try
    {
        patch.registration      = data[(int)ParamIndex::Registration];
        patch.octave_switch     = data[(int)ParamIndex::OctaveSwitch];
        patch.ensemble_effect   = data[(int)ParamIndex::EnsembleEffect];
        patch.ensemble          = data[(int)ParamIndex::Ensemble];
        patch.crescendo         = data[(int)ParamIndex::Crescendo];
        patch.release           = data[(int)ParamIndex::Release];
        patch.tone              = data[(int)ParamIndex::Tone];
        patch.tremolo           = data[(int)ParamIndex::Tremolo];
        patch.split_layer       = data[(int)ParamIndex::SplitLayer];
        patch.split_key         = data[(int)ParamIndex::SplitKey];
        patch.envelope_mode     = data[(int)ParamIndex::EnvelopeMode];
        patch.attack            = data[(int)ParamIndex::Attack];
        patch.decay             = data[(int)ParamIndex::Decay];
        patch.balance           = data[(int)ParamIndex::Balance];
        patch.animate           = data[(int)ParamIndex::Animate];
        patch.phaser            = data[(int)ParamIndex::Phaser];
        patch.reverb            = data[(int)ParamIndex::Reverb];
    }
    catch (std::exception& error)
    {
        // The received tone data seems to be invalid
        throw std::exception(std::format("setPatchFromBytes failed: {}", error.what()).c_str());
    }
}

// DSI: Streichfett
const ByteVec getDataBytes(const SoundModel::Patch& patch) noexcept
{
    ByteVec data;

    // convert Editor values to Device values
    data.clear();
    data.push_back(patch.registration.toDv());
    data.push_back(patch.octave_switch.toDv());
    data.push_back(patch.ensemble_effect.toDv());
    data.push_back(patch.ensemble.toDv());
    data.push_back(patch.crescendo.toDv());
    data.push_back(patch.release.toDv());
    data.push_back(patch.tone.toDv());
    data.push_back(patch.tremolo.toDv());
    data.push_back(patch.split_layer.toDv());
    data.push_back(patch.split_key.toDv());
    data.push_back(patch.envelope_mode.toDv());
    data.push_back(patch.attack.toDv());
    data.push_back(patch.decay.toDv());
    data.push_back(patch.balance.toDv());
    data.push_back(patch.animate.toDv());
    data.push_back(patch.phaser.toDv());
    data.push_back(patch.reverb.toDv());
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
void copyPatchAtoB(const SoundModel::Patch& a, SoundModel::Patch& b) noexcept
{
    b.registration = a.registration;
    b.octave_switch = a.octave_switch;
    b.ensemble_effect = a.ensemble_effect;
    b.ensemble = a.ensemble;
    b.crescendo = a.crescendo;
    b.release = a.release;
    b.tone = a.tone;
    b.tremolo = a.tremolo;
    b.split_layer = a.split_layer;
    b.split_key = a.split_key;
    b.envelope_mode = a.envelope_mode;
    b.attack = a.attack;
    b.decay = a.decay;
    b.balance = a.balance;
    b.animate = a.animate;
    b.phaser = a.phaser;
    b.reverb = a.reverb;
}

} // InternalPatch
} // StreichfettSse
