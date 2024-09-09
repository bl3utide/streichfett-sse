#pragma once

#include "midi/message_entity/message_entity.h"

namespace StreichfettSse
{
namespace Midi
{

class MessageStringizer final : public MessageConcept
{
    explicit MessageStringizer(const ByteVec& message)
        : MessageConcept(message)
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
