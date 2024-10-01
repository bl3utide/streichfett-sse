#pragma once

#include "midi/message_creator/message_creator.h"

namespace StreichfettSse
{
namespace Midi
{

class ChannelMessageCreator : public MessageCreator
{
public:
    virtual ~ChannelMessageCreator() {}
    ChannelMessageCreator() = delete;
    ChannelMessageCreator(const ChannelMessageCreator&) = delete;
    ChannelMessageCreator(ChannelMessageCreator&&) = delete;
    ChannelMessageCreator& operator=(const ChannelMessageCreator&) = delete;
    ChannelMessageCreator& operator=(ChannelMessageCreator&&) = delete;

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
    const ByteVec create() const override;

    NoteOffCreator() = delete;
    NoteOffCreator(const NoteOffCreator&) = delete;
    NoteOffCreator(NoteOffCreator&&) = delete;
    NoteOffCreator& operator=(const NoteOffCreator&) = delete;
    NoteOffCreator& operator=(NoteOffCreator&&) = delete;

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
    const ByteVec create() const override;

    NoteOnCreator() = delete;
    NoteOnCreator(const NoteOnCreator&) = delete;
    NoteOnCreator(NoteOnCreator&&) = delete;
    NoteOnCreator& operator=(const NoteOnCreator&) = delete;
    NoteOnCreator& operator=(NoteOnCreator&&) = delete;

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
    const ByteVec create() const override;

    ControlChangeCreator() = delete;
    ControlChangeCreator(const ControlChangeCreator&) = delete;
    ControlChangeCreator(ControlChangeCreator&&) = delete;
    ControlChangeCreator& operator=(const ControlChangeCreator&) = delete;
    ControlChangeCreator& operator=(ControlChangeCreator&&) = delete;

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
    const ByteVec create() const override;

    ProgramChangeCreator() = delete;
    ProgramChangeCreator(const ProgramChangeCreator&) = delete;
    ProgramChangeCreator(ProgramChangeCreator&&) = delete;
    ProgramChangeCreator& operator=(const ProgramChangeCreator&) = delete;
    ProgramChangeCreator& operator=(ProgramChangeCreator&&) = delete;

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
    const ByteVec create() const override;

    AllSoundOffCreator() = delete;
    AllSoundOffCreator(const AllSoundOffCreator&) = delete;
    AllSoundOffCreator(AllSoundOffCreator&&) = delete;
    AllSoundOffCreator& operator=(const AllSoundOffCreator&) = delete;
    AllSoundOffCreator& operator=(AllSoundOffCreator&&) = delete;
};

} // Midi
} // StreichfettSse
