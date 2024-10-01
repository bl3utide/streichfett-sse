#include "common.hpp"
#include "midi/midi_common.hpp"
#include "midi/message_entity/message_entity.h"

namespace StreichfettSse
{
namespace Midi
{

static MessageType getMessageType(const ByteVec bv) noexcept
{
    auto checkf = [](Byte target, Byte order)
    {
        return order <= target && target <= order + STB_MAX_CH_OFFSET;
    };

    if (checkf(bv.front(), MCV_STB_NOTE_OFF)) return MessageType::NoteOff;
    if (checkf(bv.front(), MCV_STB_NOTE_ON)) return MessageType::NoteOn;
    if (checkf(bv.front(), MCV_STB_POLY_KEYPRESS)) return MessageType::PolyphonicKeyPressure;
    if (checkf(bv.front(), MCV_STB_CONTROL_CHANGE)) return MessageType::ControlChange;
    if (checkf(bv.front(), MCV_STB_PROGRAM_CHANGE)) return MessageType::ProgramChange;
    if (checkf(bv.front(), MCV_STB_CH_PRESSURE)) return MessageType::ChannelPressure;
    if (checkf(bv.front(), MCV_STB_PITCH_WHEEL_CHG)) return MessageType::PitchWheelChange;

    if (bv.front() == MSC_STB_SYSEX && bv.back() == MSC_END_SYSEX)
    {
        return MessageType::SystemExclusive;
    }
    if (bv.front() == MSC_STB_TIMECODE) return MessageType::QuarterFrame;
    if (bv.front() == MSC_STB_SONG_POSITION) return MessageType::SongPositionPointer;
    if (bv.front() == MSC_STB_SONG_SELECT) return MessageType::SongSelect;
    if (bv.front() == MSC_STB_TUNE_REQUEST) return MessageType::TuneRequest;

    if (bv.front() == MSR_STB_CLOCK) return MessageType::TimingClock;
    if (bv.front() == MSR_STB_TICK) return MessageType::Tick;
    if (bv.front() == MSR_STB_START) return MessageType::Start;
    if (bv.front() == MSR_STB_CONTINUE) return MessageType::Continue;
    if (bv.front() == MSR_STB_STOP) return MessageType::Stop;
    if (bv.front() == MSR_STB_ACTIVE_SENSE) return MessageType::ActiveSense;
    if (bv.front() == MSR_STB_RESET) return MessageType::Reset;

    return MessageType::_UNDEFINED_;
}

static ChannelMode getChannelMode(Byte b) noexcept
{
    if (b == MCM_DTB_ALL_SOUND_OFF) return ChannelMode::AllSoundOff;
    if (b == MCM_DTB_RESET_ALL_CTRLR) return ChannelMode::ResetAllController;
    if (b == MCM_DTB_LOCAL_CTRL) return ChannelMode::LocalControl;
    if (b == MCM_DTB_ALL_NOTES_OFF) return ChannelMode::AllNotesOff;
    if (b == MCM_DTB_OMNI_OFF) return ChannelMode::OmniOff;
    if (b == MCM_DTB_OMNI_ON) return ChannelMode::OmniOn;
    if (b == MCM_DTB_MONO_MODE) return ChannelMode::MonophonicOn;
    if (b == MCM_DTB_POLY_MODE) return ChannelMode::PolyphonicOn;

    return ChannelMode::Other;
}

MessageEntity::MessageEntity(const ByteVec& message)
    : mbytes(message), channel_mode_(ChannelMode::_NONE_)
{
    type_ = getMessageType(mbytes);
    if (type_ == MessageType::ControlChange)
    {
        channel_mode_ = getChannelMode(mbytes[1]);
    }
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
