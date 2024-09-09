#pragma once

namespace StreichfettSse
{
namespace Midi
{

enum class MsgType : int
{
    // Channel-Voice
    NoteOn,
    NoteOff,
    PolyphonicKeyPressure,
    ControlChange,
    ProgramChange,
    ChannelPressure,
    PitchBendChange,

    // Channel-Mode
    AllSoundOff,
    ResetAllController,
    LocalControl,
    AllNotesOff,
    OmniOff,
    OmniOn,
    MonophonicOn,
    PolyphonicOn,

    // System-Common
    SystemExclusive,
    QuarterFrame,
    SongPositionPointer,
    SongSelect,
    TuneRequest,
    EndOfExclusive,

    // System-Realtime
    TimingClock,
    Start,
    Continue,
    Stop,
    ActiveSense,
    Reset,

    _UNDEFINED_,
};

class MessageConcept
{
public:
    explicit MessageConcept(const ByteVec& message);
    virtual ~MessageConcept() {}

    MessageConcept() = delete;
    MessageConcept(const MessageConcept&) = delete;
    MessageConcept(MessageConcept&&) = delete;
    MessageConcept& operator=(const MessageConcept&) = delete;
    MessageConcept& operator=(MessageConcept&&) = delete;

    MsgType type() const noexcept { return type_; }

protected:
    bool empty() const noexcept;
    ByteVec mbytes;

private:
    MsgType type_;
};

} // Midi
} // StreichfettSse
