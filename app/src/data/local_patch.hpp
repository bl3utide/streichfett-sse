#pragma once
#include "model/sound.hpp"

namespace StreichfettSse
{
namespace LocalPatch
{

// DSI: Streichfett
struct PatchAddress
{
    explicit PatchAddress() : sound(0) {}
    explicit PatchAddress(int s) : sound(s) {}

    int sound;      // 0-11 normally
};

extern bool current_patch_changed;

void initData() noexcept;
const PatchAddress& getCurrentPatchAddress() noexcept;
void setCurrentPatchAddress(int sound) noexcept;  // DSI: Streichfett
char getPatchBankChar(int sound) noexcept;        // DSI: Streichfett
char getPatchBankChar(const PatchAddress& patch_addr) noexcept;
int getPatchSoundNumber(int sound) noexcept;      // DSI: Streichfett
int getPatchSoundNumber(const PatchAddress& patch_addr) noexcept;
SoundModel::Patch& getOriginalPatch() noexcept;
SoundModel::Patch& getCurrentPatch() noexcept;
void setPatchFromBytes(SoundModel::Patch& patch, const ByteVec& data);
const ByteVec getDataBytes(const SoundModel::Patch& patch) noexcept;
void copyPatchAtoB(const SoundModel::Patch& a, SoundModel::Patch& b) noexcept;

} // InternalPatch
} // StreichfettSse
