#pragma once

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
const SoundAddress& getCurrentSoundAddress() noexcept;
void setCurrentSoundAddress(int sound) noexcept;  // DSI: Streichfett
char getSoundBankChar(int sound) noexcept;        // DSI: Streichfett
int getSoundPatchNumber(int sound) noexcept;      // DSI: Streichfett
SoundModel::Patch& getOriginalPatch() noexcept;
SoundModel::Patch& getCurrentPatch() noexcept;
void setPatchFromBytes(SoundModel::Patch& patch, const ByteVec& data);
const ByteVec getDataBytes(const SoundModel::Patch& patch) noexcept;
void copyPatchAtoB(const SoundModel::Patch& a, SoundModel::Patch& b) noexcept;

} // InternalPatch
} // StreichfettSse
