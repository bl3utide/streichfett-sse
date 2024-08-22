#pragma once
#include "model/sound.hpp"

namespace StreichfettSse
{
namespace Midi
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
    bool received{};
    int device_id{};
    std::string firmware_version;

    void initialize()
    {
        received = false;
    }
};

extern InquiryDump inquiry_dump;

const ByteVec getRequestDeviceInquiryMessage();
const ByteVec getRequestSoundMessage(int sound);  // DSI: Streichfett
const ByteVec getRequestGlobalMessage();
const ByteVec getProgChangeMessage(int value);
const ByteVec getAllSoundOffMessage();
bool isNoteOff(const ByteVec& mb) noexcept;
bool isNoteOn(const ByteVec& mb) noexcept;
void validateInquiryDump(const ByteVec& dump);
void validateDataDump(const ByteVec& dump, DumpType type);
const ByteVec getDataBytesFromDump(const ByteVec& dump, DumpType type);
const ByteVec getSoundDumpMessageFromPatch(int sound, const SoundModel::Patch& patch);      // DSI: Streichfett
const ByteVec getSoundParameterChangeMessage(int index, Byte value);
const std::string getByteVecString(const ByteVec& bytes);
#ifdef _DEBUG
const std::string getMessageDesc(const ByteVec& data);
#endif

} // MessageHandler
} // Midi
} // StreichfettSse
