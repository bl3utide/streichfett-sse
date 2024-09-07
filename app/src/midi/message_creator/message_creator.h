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

} // Midi
} // StreichfettSse
