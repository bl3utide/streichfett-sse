#pragma once

#include "midi/message_entity/message_entity.h"

namespace StreichfettSse
{
namespace Midi
{

class MessageStringizer : public MessageEntity
{
public:
    explicit MessageStringizer(const ByteVec& message)
        : MessageEntity(message)
    {
    }

    virtual const std::string toString() const noexcept;

    MessageStringizer() = delete;
    MessageStringizer(const MessageStringizer&) = delete;
    MessageStringizer(MessageStringizer&&) = delete;
    MessageStringizer& operator=(const MessageStringizer&) = delete;
    MessageStringizer& operator=(MessageStringizer&&) = delete;
};

class MessageDescriber final : public MessageStringizer
{
public:
    explicit MessageDescriber(const ByteVec& message)
        : MessageStringizer(message)
    {
    }

    const std::string toString() const noexcept override;

    MessageDescriber() = delete;
};

} // Midi
} // StreichfettSse
