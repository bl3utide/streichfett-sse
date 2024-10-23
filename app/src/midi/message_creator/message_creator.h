#pragma once

namespace StreichfettSse
{
namespace Midi
{

class MessageCreator
{
public:
    virtual ~MessageCreator() = default;
    virtual const ByteVec create() const = 0;

protected:
    MessageCreator() = default;
};

} // Midi
} // StreichfettSse
