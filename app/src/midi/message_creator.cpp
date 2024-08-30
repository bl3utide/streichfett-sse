#include "common.hpp"
#include "midi/midi_common.hpp"
#include "midi/message_creator.h"

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

const ByteVec ProgramChangeCreator::create() const
{
    Byte order_byte = MIDI_PC + static_cast<Byte>(ch_);

    ByteVec pc;
    pc.push_back(order_byte);
    pc.push_back(static_cast<Byte>(value_));
    return pc;
}

const ByteVec AllSoundOffCreator::create() const
{
    const Byte order_byte = MIDI_CC + static_cast<Byte>(ch_);

    ByteVec aso;
    aso.push_back(order_byte);
    aso.push_back(static_cast<Byte>(0x78));
    return aso;
}

} // Midi
} // StreichfettSse
