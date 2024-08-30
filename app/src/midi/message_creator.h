#pragma once

namespace StreichfettSse
{
namespace Midi
{

class MessageCreator
{
public:
    virtual ~MessageCreator() {}
    virtual const ByteVec create() const = 0;

    MessageCreator(const MessageCreator&) = delete;
    MessageCreator(MessageCreator&&) = delete;
    MessageCreator& operator=(const MessageCreator&) = delete;
    MessageCreator& operator=(MessageCreator&&) = delete;

protected:
    MessageCreator() {}
};

class SysExMessageCreator : MessageCreator
{
public:
    virtual ~SysExMessageCreator() {}
    SysExMessageCreator(const SysExMessageCreator&) = delete;
    SysExMessageCreator(SysExMessageCreator&&) = delete;
    SysExMessageCreator& operator=(const SysExMessageCreator&) = delete;
    SysExMessageCreator& operator=(SysExMessageCreator&&) = delete;
protected:
    SysExMessageCreator() {}
};

class RequestDeviceInquiryCreator final : SysExMessageCreator
{
public:
    explicit RequestDeviceInquiryCreator() {}
    const ByteVec create() const override;

    RequestDeviceInquiryCreator(const RequestDeviceInquiryCreator&) = delete;
    RequestDeviceInquiryCreator(RequestDeviceInquiryCreator&&) = delete;
    RequestDeviceInquiryCreator& operator=(const RequestDeviceInquiryCreator&) = delete;
    RequestDeviceInquiryCreator& operator=(RequestDeviceInquiryCreator&&) = delete;
};

class RequestGlobalCreator final : SysExMessageCreator
{
public:
    explicit RequestGlobalCreator() {}
    const ByteVec create() const override;

    RequestGlobalCreator(const RequestGlobalCreator&) = delete;
    RequestGlobalCreator(RequestGlobalCreator&&) = delete;
    RequestGlobalCreator& operator=(const RequestGlobalCreator&) = delete;
    RequestGlobalCreator& operator=(RequestGlobalCreator&&) = delete;
};

// DSI: Streichfett
class RequestSoundCreator final : SysExMessageCreator
{
public:
    explicit RequestSoundCreator(int sound)
        : sound_(sound)
    {
    }
    const ByteVec create() const override;

    RequestSoundCreator() = delete;
    RequestSoundCreator(const RequestSoundCreator&) = delete;
    RequestSoundCreator(RequestSoundCreator&&) = delete;
    RequestSoundCreator& operator=(const RequestSoundCreator&) = delete;
    RequestSoundCreator& operator=(RequestSoundCreator&&) = delete;

private:
    int sound_;
};

class ChannelMessageCreator : MessageCreator
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

class ProgramChangeCreator final : ChannelMessageCreator
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

class AllSoundOffCreator final : ChannelMessageCreator
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
