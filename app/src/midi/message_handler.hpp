#pragma once

#include "model/sound.hpp"

namespace StreichfettSse
{
namespace MessageHandler
{

using Bytes = std::vector<unsigned char>;

enum class DumpType : int
{
    Sound,
    Global,
};

// DSI: Streichfett
struct InquiryDump
{
    bool received;
    int device_id;
    std::string firmware_version;

    void initialize()
    {
        received = false;
    }
};

extern InquiryDump inquiry_dump;

Bytes getInquiryRequestMessage();
Bytes getSoundRequestMessage(const int sound);  // DSI: Streichfett
Bytes getGlobalRequestMessage();
Bytes getProgChangeMessage(const int value);
Bytes getAllSoundOffMessage();
bool isNoteOff(const Bytes& mb);
bool isNoteOn(const Bytes& mb);
bool checkInquiryDump(const Bytes& dump);
void checkDump(const Bytes& dump, const DumpType type);
Bytes getDataBytesFromDump(const Bytes& dump, const DumpType type);
Bytes getSoundDumpMessageFromPatch(const int sound, const SoundModel::Patch* const patch);      // DSI: Streichfett
//Bytes getSoundParameterChangeMessage(const int index, const int value);   // TODO delete toDvFunc
Bytes getSoundParameterChangeMessage(const int index, const unsigned char value);
#ifdef _DEBUG
std::string getMessageDesc(const Bytes& data);
#endif

} // MessageHandler
} // StreichfettSse
