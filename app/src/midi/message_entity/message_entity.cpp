#include "common.hpp"
#include "midi/midi_common.hpp"
#include "midi/message_entity/message_entity.h"

namespace StreichfettSse
{
namespace Midi
{

static MsgType getMsgType(Byte b) noexcept
{
    auto checkf = [](Byte target, Byte order)
    {
        return order <= target && target <= order + MAX_CH_OFFSET;
    };

    if (checkf(b, MIDI_NOTE_OFF)) return MsgType::NoteOff;
    if (checkf(b, MIDI_NOTE_ON)) return MsgType::NoteOn;
    if (checkf(b, MIDI_PC)) return MsgType::ProgramChange;
    if (checkf(b, MIDI_CC)) return MsgType::ControlChange;

    return MsgType::_UNDEFINED_;
}

MessageEntity::MessageEntity(const ByteVec& message)
    : mbytes(message)
{
    type_ = getMsgType(mbytes[0]);
}

bool MessageEntity::empty() const noexcept
{
    return mbytes.empty();
}

bool MessageEntity::isSysEx() const noexcept
{
    return mbytes.front() == MSC_STB_SYSEX && mbytes.back() == MSC_END_SYSEX;
}

} // Midi
} // StreichfettSse
