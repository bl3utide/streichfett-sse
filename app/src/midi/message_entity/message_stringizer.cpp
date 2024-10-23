#include "common.hpp"
#include "midi/midi_common.hpp"
#include "midi/message_entity/message_entity.h"
#include "midi/message_entity/message_stringizer.h"

namespace StreichfettSse
{
namespace Midi
{

const std::string MessageStringizer::toString() const noexcept
{
    std::stringstream ss;

    const auto entity_size = entity->size();
    for (auto e_i = 0; e_i < entity_size; ++e_i)
    {
        ss << "0x"
            << std::uppercase
            << std::setfill('0')
            << std::setw(2)
            << std::hex
            << static_cast<int>(entity->at(e_i)) << " ";
    }

    const auto str = ss.str();

    return str.substr(0, str.size() - 1);
}

const std::string MessageDescriber::toString() const noexcept
{
    std::stringstream ss;
    const auto msg_type = entity->type();

    if (entity->empty())
    {
        ss << "Empty Message";
    }
    else if (msg_type == MessageType::NoteOff || msg_type == MessageType::NoteOn)
    {
        if (msg_type == MessageType::NoteOff) ss << "Note Off";
        else ss << "Note On";

        ss << " <" << static_cast<int>(entity->at(1))
            << "> Vel(" << static_cast<int>(entity->at(2))
            << ")";
    }
    else if (msg_type == MessageType::ControlChange)
    {
        const auto cc_type = entity->at(1);
        const auto cc_value = static_cast<int>(entity->at(1));
        if      (cc_type == 0x00) ss << "Bank Select MSB: " << cc_value;
        else if (cc_type == 0x01) ss << "Modulation: " << cc_value;
        else if (cc_type == 0x20) ss << "Bank Select LSB: " << cc_value;
        else if (cc_type == 0x40) ss << "Damper Pedal: " << cc_value;
        else if (cc_type == 0x78) ss << "All Sound Off";
        else if (cc_type == 0x79) ss << "Reset All Controllers";
        else if (cc_type == 0x7A)
        {
            ss << "Local Control";
            if (cc_value == 0)        ss << " Off";
            else if (cc_value == 127) ss << " On";
            else                      ss << " (unknown 3rd byte)";
        }
        else if (cc_type == 0x7B) ss << "All Notes Off";
        else if (cc_type == 0x7C) ss << "Omni Mode Off";
        else if (cc_type == 0x7D) ss << "Omni Mode On";
        else
        {
            ss << "Control Change (" << static_cast<int>(cc_type) << "): "
                << cc_value;
        }
    }
    else if (msg_type == MessageType::ProgramChange)
    {
        ss << "Program Change (" << static_cast<int>(entity->at(1)) << ")";
    }
    else if (entity->isSysEx())
    {
        ss << "SysEx: ";
        if (entity->at(3) == ORDER_GENERAL_INFO)
        {
            if (entity->at(4) == ORDER_INQUIRY_REQUEST)
            {
                ss << "Identity Request";
            }
            else if (entity->at(4) == ORDER_INQUIRY_RESPONSE)
            {
                ss << "Identity Response";
            }
            else
            {
                ss << "(unknown order)";
            }
        }
        else if (entity->at(4) == ORDER_SOUND_REQUEST)
        {
            ss << "Sound Request (";
            if (entity->at(5) == SOUND_EDIT_BUFFER) ss << "edit buffer)";
            else ss << static_cast<int>(entity->at(5)) << ")";
        }
        else if (entity->at(4) == ORDER_GLOBAL_REQUEST)
        {
            ss << "Global Request";
        }
        else if (entity->at(4) == ORDER_SOUND_DUMP)
        {
            ss << "Sound Dump (";
            if (entity->at(5) == SOUND_EDIT_BUFFER) ss << "edit buffer)";
            else ss << static_cast<int>(entity->at(5)) << ")";
        }
        else if (entity->at(4) == ORDER_GLOBAL_DUMP)
        {
            ss << "Global Dump";
        }
        else
        {
            ss << "Device specific or other message";
        }
    }
    else
    {
        ss << "?";
    }

    return ss.str();
}

} // Midi
} // StreichfettSse
