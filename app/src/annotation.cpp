#include "common.hpp"
#include "annotation.hpp"

namespace StreichfettSse
{
namespace Annotation
{

// private
std::string _text;
Type _type;
const std::map<Message, std::string> MESSAGES {
    {Message::DisconnectedAlert, "Please set the MIDI connections from OPTION -> Synth Input/Output Device."},
};

const std::string& getText() noexcept
{
    return _text;
}

Type getType() noexcept
{
    return _type;
}

void setText(const Message message, const Type type) noexcept
{
    _text = MESSAGES.at(message);
    _type = type;
}

void setText(const std::string& message, const Type type) noexcept
{
    _text = message;
    _type = type;
}

void clearText() noexcept
{
    _text.clear();
}

} // Annotation
} // StreichfettSse
