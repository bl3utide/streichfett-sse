#pragma once

#include "midi/msg/message_creator.h"

namespace StreichfettSse
{
namespace Midi
{

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

} // Midi
} // StreichfettSse
