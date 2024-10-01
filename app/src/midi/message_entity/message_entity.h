#pragma once

namespace StreichfettSse
{
namespace Midi
{

enum class MessageType : int
{
    // Channel-Voice
    NoteOn,
    NoteOff,
    PolyphonicKeyPressure,
    ControlChange,
    ProgramChange,
    ChannelPressure,
    PitchBendChange,

    // System-Common
    SystemExclusive,
    QuarterFrame,
    SongPositionPointer,
    SongSelect,
    TuneRequest,

    // System-Realtime
    TimingClock,
    Start,
    Continue,
    Stop,
    ActiveSense,
    Reset,

    _UNDEFINED_,
};

enum class ChannelMode : int
{
    AllSoundOff,
    ResetAllController,
    LocalControl,
    AllNotesOff,
    OmniOff,
    OmniOn,
    MonophonicOn,
    PolyphonicOn,
    Other,

    _NONE_,
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

    MessageType type() const noexcept { return type_; }
    ChannelMode channelMode() const noexcept { return channel_mode_; }

protected:
    bool empty() const noexcept;
    bool isSysEx() const noexcept;
    ByteVec mbytes;

private:
    MessageType type_;
    ChannelMode channel_mode_;
};

} // Midi
} // StreichfettSse
