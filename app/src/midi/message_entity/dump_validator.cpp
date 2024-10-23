#include "common.hpp"
#include "midi/midi_common.hpp"
#include "midi/message_entity/message_entity.h"
#include "midi/message_entity/dump_validator.h"

namespace StreichfettSse
{
namespace Midi
{

void DumpValidator::fail(std::string_view cause_desc) const
{
    throw std::exception(std::format("{1} failed: {2)", name_, cause_desc).c_str());
}

void DumpValidator::checkValidated() const
{
    if (!is_valid_)
    {
        fail("not validated yet");
    }
}

void DumpValidator::setValidated() noexcept
{
    is_valid_ = true;
}

void DumpValidator::validateSysEx() const
{
    if (entity->empty()) fail("empty data");

    if (!entity->isSysEx()) fail("not SysEx");
}

// DSI: Streichfett
void DataDumpValidator::validateDataSysEx() const
{
    validateSysEx();

    // Waldorf Music Manufacturer ID
    if (entity->at(1) != DEVICE_MANUFACTURER_ID)
    {
        fail("SysEx has incorrect manufacturer id");
    }

    // Device Family Code
    if (entity->at(2) != DEVICE_FAMILY_CODE)
    {
        fail("SysEx has incorrect device family code");
    }
}

// DSI: Streichfett
void DeviceInquiryDumpValidator::validate()
{
    validateSysEx();

    if (entity->size() != INQUIRY_DUMP_SIZE)
    {
        fail("SysEx byte length");
    }

    // Universal SysEx Header
    if (entity->at(1) != SYSEX_IDNUM_NONREALTIME)
    {
        fail("not Universal Non-Real Time SysEx");
    }

    // Device Inquiry Dump
    if (entity->at(3) != ORDER_GENERAL_INFO ||
        entity->at(4) != ORDER_INQUIRY_RESPONSE)
    {
        fail("not device inquiry dump");
    }

    // Waldorf Music Manufacturer ID
    if (entity->at(5) != DEVICE_MANUFACTURER_ID)
    {
        fail("SysEx has incorrect manufacturer id");
    }

    // Device Family Code
    if (entity->at(6) != DEVICE_FAMILY_CODE)
    {
        fail("SysEx has incorrect device family code");
    }

    setValidated();
}

// DSI: Streichfett
void GlobalDumpValidator::validate()
{
    validateDataSysEx();

    if (entity->size() != GLOBAL_DUMP_SIZE)
    {
        fail("SysEx byte length");
    }

    if (entity->at(4) != ORDER_GLOBAL_DUMP)
    {
        fail("not global dump SysEx");
    }

    setValidated();
}

// DSI: Streichfett
void SoundDumpValidator::validate()
{
    validateDataSysEx();

    if (entity->size() != SOUND_DUMP_SIZE)
    {
        fail("SysEx byte length");
    }

    if (entity->at(4) != ORDER_SOUND_DUMP)
    {
        fail("not sound dump SysEx");
    }

    setValidated();
}

} // Midi
} // StreichfettSse
