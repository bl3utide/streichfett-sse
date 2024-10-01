#include "common.hpp"
#include "midi/midi_common.hpp"
#include "midi/message_creator/channel_message_creator.h"

namespace StreichfettSse
{
namespace Midi
{

const ByteVec NoteOffCreator::create() const
{
    const Byte order_byte = MCV_STB_NOTE_OFF + static_cast<Byte>(ch_);

    ByteVec note;
    note.push_back(order_byte);
    note.push_back(static_cast<Byte>(note_));
    note.push_back(static_cast<Byte>(vel_));
    return note;
}

const ByteVec NoteOnCreator::create() const
{
    const Byte order_byte = MCV_STB_NOTE_ON + static_cast<Byte>(ch_);

    ByteVec note;
    note.push_back(order_byte);
    note.push_back(static_cast<Byte>(note_));
    note.push_back(static_cast<Byte>(vel_));
    return note;
}

const ByteVec ControlChangeCreator::create() const
{
    const Byte order_byte = MCV_STB_CONTROL_CHANGE + static_cast<Byte>(ch_);

    ByteVec cc;
    cc.push_back(order_byte);
    cc.push_back(static_cast<Byte>(cc_index_));
    cc.push_back(static_cast<Byte>(value_));
    return cc;
}

const ByteVec ProgramChangeCreator::create() const
{
    const Byte order_byte = MCV_STB_PROGRAM_CHANGE + static_cast<Byte>(ch_);

    ByteVec pc;
    pc.push_back(order_byte);
    pc.push_back(static_cast<Byte>(value_));
    return pc;
}

const ByteVec AllSoundOffCreator::create() const
{
    const Byte order_byte = MCV_STB_CONTROL_CHANGE + static_cast<Byte>(ch_);

    ByteVec aso;
    aso.push_back(order_byte);
    aso.push_back(static_cast<Byte>(MCM_DTB_ALL_SOUND_OFF));
    return aso;
}

} // Midi
} // StreichfettSse
