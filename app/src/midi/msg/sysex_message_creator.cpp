#include "common.hpp"
#include "model/sound.hpp"
#include "midi/midi_common.hpp"
#include "midi/msg/sysex_message_creator.h"

namespace StreichfettSse
{
namespace Midi
{

const ByteVec RequestDeviceInquiryCreator::create() const
{
    ByteVec m;
    m.push_back(SYSEX_FIRST);
    m.push_back(SYSEX_SECOND_UNRT);
    m.push_back(DEVICE_ID_OMNI);
    // Identity Request (2 bytes)
    m.push_back(ORDER_INQUIRY);
    m.push_back(ORDER_INQUIRY_REQUEST);
    m.push_back(SYSEX_LAST);
    return m;
}

// DSI: Streichfett
const ByteVec RequestGlobalCreator::create() const
{
    ByteVec m;
    m.push_back(SYSEX_FIRST);
    m.push_back(DEVICE_MANUFACTURER_ID);
    m.push_back(DEVICE_FAMILY_CODE);
    m.push_back(DEVICE_ID_OMNI);
    m.push_back(ORDER_GLOBAL_REQUEST);
    m.push_back(0x00);    // Reserved
    m.push_back(SYSEX_LAST);
    return m;
}

// DSI: Streichfett
const ByteVec RequestSoundCreator::create() const
{
    ByteVec m;
    m.push_back(SYSEX_FIRST);
    m.push_back(DEVICE_MANUFACTURER_ID);
    m.push_back(DEVICE_FAMILY_CODE);
    m.push_back(DEVICE_ID_OMNI);
    m.push_back(ORDER_SOUND_REQUEST);
    m.push_back(static_cast<Byte>(sound_));
    m.push_back(SYSEX_LAST);
    return m;
}

const ByteVec SoundDumpCreator::create() const
{
    // TODO impl
    return ByteVec();
}

} // Midi
} // StreichfettSse
