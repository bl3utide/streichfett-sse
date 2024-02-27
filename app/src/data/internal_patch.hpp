#pragma once

#include "midi/message_handler.hpp"
#include "model/global.hpp"
#include "model/sound.hpp"

namespace StreichfettSse
{
namespace InternalPatch
{

// DSI: Streichfett
struct SoundAddress
{
    int sound;      // 0-11 normally

    SoundAddress()
    {
        sound = 0;
    }

    SoundAddress(const int s)
    {
        sound = s;
    }
};

extern bool current_patch_changed;

void initData() noexcept;
SoundAddress* getCurrentSoundAddress() noexcept;
void setCurrentSoundAddress(const int sound) noexcept;  // DSI: Streichfett
char getSoundBankChar(const int sound) noexcept;        // DSI: Streichfett
int getSoundPatchNumber(const int sound) noexcept;      // DSI: Streichfett
SoundModel::Patch* getOriginalPatch() noexcept;
SoundModel::Patch* getCurrentPatch() noexcept;
void setPatchFromBytes(SoundModel::Patch* patch, const MessageHandler::Bytes& data);
MessageHandler::Bytes getDataBytes(const SoundModel::Patch* const patch);
void copyPatchAtoB(SoundModel::Patch* a, SoundModel::Patch* b);

} // InternalPatch
} // StreichfettSse
