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
    PitchWheelChange,

    // System-Common
    SystemExclusive,
    QuarterFrame,
    SongPositionPointer,
    SongSelect,
    TuneRequest,

    // System-Realtime
    TimingClock,
    Tick,
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
    virtual ~MessageEntity() = default;
    MessageType type() const noexcept { return type_; }
    ChannelMode channelMode() const noexcept { return channel_mode_; }
    bool empty() const noexcept { return mbytes.empty(); }
    bool isSysEx() const noexcept;
    size_t size() const noexcept { return mbytes.size(); }
    Byte at(int index = 0) const;

protected:
    const ByteVec mbytes;

private:
    MessageType type_;
    ChannelMode channel_mode_;
};

} // Midi
} // StreichfettSse
