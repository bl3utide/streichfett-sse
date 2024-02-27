#pragma once

namespace StreichfettSse
{
namespace Annotation
{

enum class Type : int
{
    General,
    Alert,
    _COUNT_,
};

enum class Message : int
{
    DisconnectedAlert,
    _COUNT_,
};

const std::string& getText() noexcept;
Type getType() noexcept;
void setText(const Message message, const Type type);
void setText(const std::string& message, const Type type);
void clearText() noexcept;

} // Annotation
} // StreichfettSse
