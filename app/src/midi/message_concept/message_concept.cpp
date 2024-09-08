#include "common.hpp"
#include "midi/midi_common.hpp"
#include "midi/message_concept/message_concept.h"

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

MessageConcept::MessageConcept(const ByteVec& message)
    : mbytes(message)
{
    type_ = getMsgType(mbytes[0]);
}

bool MessageConcept::empty() const noexcept
{
    return mbytes.empty();
}

} // Midi
} // StreichfettSse
