#pragma once

namespace StreichfettSse
{
namespace Midi
{

class MessageEntity;

class MessageStringizer
{
public:
    explicit MessageStringizer(const ByteVec& message)
        : entity(std::make_unique<MessageEntity>(message))
    {
    }
    virtual ~MessageStringizer() = default;
    virtual const std::string toString() const noexcept;

protected:
    std::unique_ptr<MessageEntity> entity;
};

class MessageDescriber final : public MessageStringizer
{
public:
    explicit MessageDescriber(const ByteVec& message)
        : MessageStringizer(message)
    {
    }
    const std::string toString() const noexcept override;
};

} // Midi
} // StreichfettSse
