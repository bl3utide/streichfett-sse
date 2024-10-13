#pragma once

#include "midi/message_creator/message_creator.h"

namespace StreichfettSse
{
namespace Midi
{

class ChannelMessageCreator : public MessageCreator
{
public:
    ChannelMessageCreator() = delete;
    virtual ~ChannelMessageCreator() = default;

protected:
    ChannelMessageCreator(int midi_ch)
        : ch_(midi_ch)
    {
    }

    int ch_;
};

class NoteOffCreator final : public ChannelMessageCreator
{
public:
    explicit NoteOffCreator(int midi_ch, int note_num, int velocity)
        : ChannelMessageCreator(midi_ch), note_(note_num), vel_(velocity)
    {
    }
    NoteOffCreator() = delete;
    const ByteVec create() const override;

private:
    int note_;
    int vel_;
};

class NoteOnCreator final : public ChannelMessageCreator
{
public:
    explicit NoteOnCreator(int midi_ch, int note_num, int velocity)
        : ChannelMessageCreator(midi_ch), note_(note_num), vel_(velocity)
    {
    }
    NoteOnCreator() = delete;
    const ByteVec create() const override;

private:
    int note_;
    int vel_;
};

class ControlChangeCreator final : public ChannelMessageCreator
{
public:
    explicit ControlChangeCreator(int midi_ch, int cc, int value)
        : ChannelMessageCreator(midi_ch), cc_index_(cc), value_(value)
    {
    }
    ControlChangeCreator() = delete;
    const ByteVec create() const override;

private:
    int cc_index_;
    int value_;
};

class ProgramChangeCreator final : public ChannelMessageCreator
{
public:
    explicit ProgramChangeCreator(int midi_ch, int value)
        : ChannelMessageCreator(midi_ch), value_(value)
    {
    }
    ProgramChangeCreator() = delete;
    const ByteVec create() const override;

private:
    int value_;
};

class AllSoundOffCreator final : public ChannelMessageCreator
{
public:
    explicit AllSoundOffCreator(int midi_ch)
        : ChannelMessageCreator(midi_ch)
    {
    }
    AllSoundOffCreator() = delete;
    const ByteVec create() const override;
};

} // Midi
} // StreichfettSse
