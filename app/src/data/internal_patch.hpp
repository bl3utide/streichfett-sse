#pragma once

#include "model/global.hpp"
#include "model/sound.hpp"

namespace StreichfettSse
{
namespace InternalPatch
{

// DSI: Streichfett
struct SoundAddress
{
    explicit SoundAddress() : sound(0) {}
    explicit SoundAddress(int s) : sound(s) {}

    int sound;      // 0-11 normally
};

extern bool current_patch_changed;

void initData() noexcept;
SoundAddress* getCurrentSoundAddress() noexcept;
void setCurrentSoundAddress(int sound) noexcept;  // DSI: Streichfett
char getSoundBankChar(int sound) noexcept;        // DSI: Streichfett
int getSoundPatchNumber(int sound) noexcept;      // DSI: Streichfett
SoundModel::Patch* getOriginalPatch() noexcept;
SoundModel::Patch* getCurrentPatch() noexcept;
void setPatchFromBytes(SoundModel::Patch* patch, const ByteVec& data);
ByteVec getDataBytes(const SoundModel::Patch* const patch);
void copyPatchAtoB(SoundModel::Patch* a, SoundModel::Patch* b);

} // InternalPatch
} // StreichfettSse
