#pragma once

#include "model/sound.hpp"

namespace StreichfettSse
{
namespace MessageHandler
{

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

ByteVec getInquiryRequestMessage();
ByteVec getSoundRequestMessage(const int sound);  // DSI: Streichfett
ByteVec getGlobalRequestMessage();
ByteVec getProgChangeMessage(const int value);
ByteVec getAllSoundOffMessage();
bool isNoteOff(const ByteVec& mb);
bool isNoteOn(const ByteVec& mb);
bool checkInquiryDump(const ByteVec& dump);
void checkDump(const ByteVec& dump, const DumpType type);
ByteVec getDataBytesFromDump(const ByteVec& dump, const DumpType type);
ByteVec getSoundDumpMessageFromPatch(const int sound, const SoundModel::Patch* const patch);      // DSI: Streichfett
//ByteVec getSoundParameterChangeMessage(const int index, const int value);   // TODO delete toDvFunc
ByteVec getSoundParameterChangeMessage(const int index, const Byte value);
std::string getByteVecString(const ByteVec& bytes);
#ifdef _DEBUG
std::string getMessageDesc(const ByteVec& data);
#endif

} // MessageHandler
} // StreichfettSse
