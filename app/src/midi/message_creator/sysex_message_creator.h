#pragma once

#include "midi/message_creator/message_creator.h"

namespace StreichfettSse
{
namespace Midi
{

class RequestDeviceInquiryCreator final : public MessageCreator
{
public:
    explicit RequestDeviceInquiryCreator() = default;
    const ByteVec create() const override;
};

class RequestGlobalCreator final : public MessageCreator
{
public:
    explicit RequestGlobalCreator() = default;
    const ByteVec create() const override;
};

// DSI: Streichfett
class RequestSoundCreator final : public MessageCreator
{
public:
    explicit RequestSoundCreator(int sound)
        : sound_(sound)
    {
    }
    RequestSoundCreator() = delete;
    const ByteVec create() const override;

private:
    int sound_;
};

class SoundDumpCreator final : public MessageCreator
{
public:
    explicit SoundDumpCreator(int sound, const SoundModel::Patch& patch)
        : sound_(sound), patch_(patch)
    {
    }
    SoundDumpCreator() = delete;
    const ByteVec create() const override;

private:
    SoundModel::Patch patch_;
    int sound_;
};

} // Midi
} // StreichfettSse
