#pragma once
#include "model/sound.hpp"

// TODO いずれ消えるモジュール

namespace StreichfettSse
{
namespace Midi
{
namespace ErstwhileMessageHandler
{

//enum class DumpType : int
//{
//    Sound,
//    Global,
//};

// TODO move -> message_generator
//const ByteVec getRequestDeviceInquiryMessage();
//const ByteVec getRequestSoundMessage(int sound);  // DSI: Streichfett
//const ByteVec getRequestGlobalMessage();
//const ByteVec getProgChangeMessage(int value);
//const ByteVec getAllSoundOffMessage();
// TODO move -> ?
bool isNoteOff(const ByteVec& mb) noexcept;
bool isNoteOn(const ByteVec& mb) noexcept;
//void validateInquiryDump(const ByteVec& dump);
//void validateDataDump(const ByteVec& dump, DumpType type);
//const ByteVec getDataBytesFromDump(const ByteVec& dump, DumpType type);
// TODO move -> message_generator
const ByteVec getSoundDumpMessageFromPatch(int sound, const SoundModel::Patch& patch);      // DSI: Streichfett
//const ByteVec getSoundParameterChangeMessage(int index, Byte value);
// TODO move -> message_stringizer
const std::string getByteVecString(const ByteVec& bytes);
#ifdef _DEBUG
const std::string getMessageDesc(const ByteVec& data);
#endif

} // ErstwhileMessageHandler
} // Midi
} // StreichfettSse
