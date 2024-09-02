#include "common.hpp"
#include "midi/midi_common.hpp"
#include "midi/msg/channel_message_creator.h"

namespace StreichfettSse
{
namespace Midi
{

const ByteVec ControlChangeCreator::create() const
{
    const Byte order_byte = MIDI_CC + static_cast<Byte>(ch_);

    ByteVec cc;
    cc.push_back(MIDI_CC + static_cast<Byte>(ch_));
    cc.push_back(static_cast<Byte>(cc_index_));
    cc.push_back(static_cast<Byte>(value_));
    return cc;
}

const ByteVec ProgramChangeCreator::create() const
{
    const Byte order_byte = MIDI_PC + static_cast<Byte>(ch_);

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
