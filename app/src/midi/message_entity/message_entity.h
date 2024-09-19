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

class MessageEntity
{
public:
    explicit MessageEntity(const ByteVec& message);
    virtual ~MessageEntity() {}

    MessageEntity() = delete;
    MessageEntity(const MessageEntity&) = delete;
    MessageEntity(MessageEntity&&) = delete;
    MessageEntity& operator=(const MessageEntity&) = delete;
    MessageEntity& operator=(MessageEntity&&) = delete;

    MsgType type() const noexcept { return type_; }

protected:
    bool empty() const noexcept;
    ByteVec mbytes;

private:
    MsgType type_;
};

} // Midi
} // StreichfettSse
