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

class MidiMessage
{
public:
    explicit MidiMessage(const ByteVec& message);
    virtual ~MidiMessage() {}

    MidiMessage() = delete;
    MidiMessage(const MidiMessage&) = delete;
    MidiMessage(MidiMessage&&) = delete;
    MidiMessage& operator=(const MidiMessage&) = delete;
    MidiMessage& operator=(MidiMessage&&) = delete;

    MsgType type() const noexcept { return type_; }

protected:
    bool empty() const noexcept;
    ByteVec mbytes;

private:
    MsgType type_;
};

class MessageStringizer final : public MidiMessage
{
    explicit MessageStringizer(const ByteVec& message)
        : MidiMessage(message)
    {
    }

    // TODO string format option?
    //  -> strinf formatってなんだっけ・・・
    const std::string toString() const noexcept;
    const std::string describe() const noexcept;

    MessageStringizer() = delete;
    MessageStringizer(const MessageStringizer&) = delete;
    MessageStringizer(MessageStringizer&&) = delete;
    MessageStringizer& operator=(const MessageStringizer&) = delete;
    MessageStringizer& operator=(MessageStringizer&&) = delete;
};

} // Midi
} // StreichfettSse
