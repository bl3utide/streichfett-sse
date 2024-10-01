#include "common.hpp"
#include "midi/midi_common.hpp"
#include "midi/message_entity/dump_message.h"

namespace StreichfettSse
{
namespace Midi
{

void DumpMessage::fail(std::string_view cause_desc) const
{
    throw std::exception(std::format("{1} failed: {2)", name_, cause_desc).c_str());
}

void DumpMessage::checkValidated() const
{
    if (!is_validated_) failCauseNotValidated();
}

void DumpMessage::setValidated() noexcept
{
    is_validated_ = true;
}

void DumpMessage::failCauseNotValidated() const
{
    fail("not validated yet");
}

void DumpMessage::validateSysEx() const
{
    if (empty()) fail("empty data");

    if (!isSysEx()) fail("not SysEx");
}

const ByteVec DataDumpMessage::getDataBytes() const
{
    checkValidated();

    ByteVec data;

    for (auto i = di_first_; i <= di_last_; ++i)
    {
        data.push_back(mbytes[i]);
    }

    return data;
}

// DSI: Streichfett
void DataDumpMessage::validateDataSysEx() const
{
    validateSysEx();

    // Waldorf Music Manufacturer ID
    if (mbytes[1] != DEVICE_MANUFACTURER_ID)
    {
        fail("SysEx has incorrect manufacturer id");
    }

    // Device Family Code
    if (mbytes[2] != DEVICE_FAMILY_CODE)
    {
        fail("SysEx has incorrect device family code");
    }
}

// DSI: Streichfett
void DeviceInquiryDumpMessage::validate()
{
    validateSysEx();

    if (mbytes.size() != INQUIRY_DUMP_SIZE)
    {
        fail("SysEx byte length");
    }

    // Universal SysEx Header
    if (mbytes[1] != SYSEX_IDNUM_NONREALTIME)
    {
        fail("not Universal Non-Real Time SysEx");
    }

    // Device Inquiry Dump
    if (mbytes[3] != ORDER_GENERAL_INFO || mbytes[4] != ORDER_INQUIRY_RESPONSE)
    {
        fail("not device inquiry dump");
    }

    // Waldorf Music Manufacturer ID
    if (mbytes[5] != DEVICE_MANUFACTURER_ID)
    {
        fail("SysEx has incorrect manufacturer id");
    }

    // Device Family Code
    if (mbytes[6] != DEVICE_FAMILY_CODE)
    {
        fail("SysEx has incorrect device family code");
    }

    setValidated();
}

// DSI: Streichfett
const DeviceInquiryResult DeviceInquiryDumpMessage::getResult() const
{
    checkValidated();

    const auto device_id = static_cast<int>(mbytes[2]);
    const auto firmware_version = std::format("{0}.{1}", mbytes[10], mbytes[11]);
    return DeviceInquiryResult(device_id, firmware_version);
}

// DSI: Streichfett
void GlobalDumpMessage::validate()
{
    validateDataSysEx();

    if (mbytes.size() != GLOBAL_DUMP_SIZE)
    {
        fail("SysEx byte length");
    }

    if (mbytes[4] != ORDER_GLOBAL_DUMP)
    {
        fail("not global dump SysEx");
    }

    setValidated();
}

// DSI: Streichfett
void SoundDumpMessage::validate()
{
    validateDataSysEx();

    if (mbytes.size() != SOUND_DUMP_SIZE)
    {
        fail("SysEx byte length");
    }

    if (mbytes[4] != ORDER_SOUND_DUMP)
    {
        fail("not sound dump SysEx");
    }

    setValidated();
}

} // Midi
} // StreichfettSse
