#pragma once

#include "midi/msg/message_handler.h"

namespace StreichfettSse
{
namespace Midi
{

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
