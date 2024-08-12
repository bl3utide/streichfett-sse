#include "common.hpp"
#include "data/internal_patch.hpp"
#include "data/internal_setting.hpp"
#include "midi/message_handler.hpp"

namespace StreichfettSse
{
namespace MessageHandler
{

// public
InquiryDump inquiry_dump;

// private
// ------------ common ------------
const Byte SYSEX_FIRST          = 0xF0;
const Byte SYSEX_LAST           = 0xF7;
const Byte SYSEX_SECOND_UNRT    = 0x7E;
const Byte DEVICE_ID_OMNI       = 0x7F;
const Byte MIDI_NOTE_OFF        = 0x80;
const Byte MIDI_NOTE_ON         = 0x90;
const Byte MIDI_CC              = 0xB0;
const Byte MIDI_PC              = 0xC0;
const Byte MAX_CH_OFFSET        = 0x0F;
// ------------ order ------------
// DSI: Streichfett
const Byte ORDER_INQUIRY            = 0x06;
const Byte ORDER_INQUIRY_REQUEST    = 0x01;
const Byte ORDER_INQUIRY_RESPONSE   = 0x02;
const Byte ORDER_SOUND_REQUEST      = 0x00;
const Byte ORDER_SOUND_DUMP         = 0x10;
const Byte ORDER_GLOBAL_REQUEST     = 0x01;
const Byte ORDER_GLOBAL_DUMP        = 0x11;
// ------------ parameter ------------
// DSI: Streichfett
const Byte DEVICE_MANUFACTURER_ID   = 0x3E;     // Waldorf
const Byte DEVICE_FAMILY_CODE       = 0x19;     // Streichfett
const Byte SOUND_EDIT_BUFFER        = 0x7F;
// ------------ data ------------
// DSI: Streichfett
const size_t INQUIRY_DUMP_SIZE      = 13;
const int SOUND_DATA_INDEX_FIRST    = 6;
const int SOUND_DATA_INDEX_LAST     = 29;
const int GLOBAL_DATA_INDEX_FIRST   = 6;
const int GLOBAL_DATA_INDEX_LAST    = 13;

static bool isSysex(const ByteVec& mb) noexcept
{
    return mb.front() == SYSEX_FIRST && mb.back() == SYSEX_LAST;
}

const ByteVec getInquiryRequestMessage()
{
    ByteVec req;
    req.clear();
    req.push_back(SYSEX_FIRST);
    req.push_back(SYSEX_SECOND_UNRT);
    req.push_back(DEVICE_ID_OMNI);
    // Identity Request (2 bytes)
    req.push_back(ORDER_INQUIRY);
    req.push_back(ORDER_INQUIRY_REQUEST);
    req.push_back(SYSEX_LAST);
    return req;
}

// DSI: Streichfett
const ByteVec getSoundRequestMessage(int sound)
{
    ByteVec req;
    req.clear();
    req.push_back(SYSEX_FIRST);
    req.push_back(DEVICE_MANUFACTURER_ID);
    req.push_back(DEVICE_FAMILY_CODE);
    req.push_back(DEVICE_ID_OMNI);
    req.push_back(ORDER_SOUND_REQUEST);
    req.push_back(static_cast<Byte>(sound));
    req.push_back(SYSEX_LAST);
    return req;
}

// DSI: Streichfett
const ByteVec getGlobalRequestMessage()
{
    ByteVec req;
    req.clear();
    req.push_back(SYSEX_FIRST);
    req.push_back(DEVICE_MANUFACTURER_ID);
    req.push_back(DEVICE_FAMILY_CODE);
    req.push_back(DEVICE_ID_OMNI);
    req.push_back(ORDER_GLOBAL_REQUEST);
    req.push_back(0x00);    // Reserved
    req.push_back(SYSEX_LAST);
    return req;
}

const ByteVec getProgChangeMessage(int value)
{
    const auto ch = InternalSetting::getDeviceMidiChannel();
    Byte order_byte = MIDI_PC + static_cast<Byte>(ch);

    ByteVec pc;
    pc.clear();
    pc.push_back(order_byte);
    pc.push_back(static_cast<Byte>(value));
    return pc;
}

const ByteVec getAllSoundOffMessage()
{
    const auto ch = InternalSetting::getDeviceMidiChannel();
    const Byte order_byte = MIDI_CC + static_cast<Byte>(ch);

    ByteVec aso;
    aso.clear();
    aso.push_back(order_byte);
    aso.push_back(static_cast<Byte>(0x78));
    return aso;
}

bool isNoteOff(const ByteVec& mb) noexcept
{
    return MIDI_NOTE_OFF <= mb[0] && mb[0] <= MIDI_NOTE_OFF + MAX_CH_OFFSET;
}

bool isNoteOn(const ByteVec& mb) noexcept
{
    return MIDI_NOTE_ON <= mb[0] && mb[0] <= MIDI_NOTE_ON + MAX_CH_OFFSET;
}

// DSI: Streichfett
bool checkInquiryDump(const ByteVec& dump)
{
    if (dump.size() != INQUIRY_DUMP_SIZE) return false;

    if (!isSysex(dump)) return false;

    // Universal SysEx Header
    if (dump[1] != SYSEX_SECOND_UNRT) return false;

    // Device Inquiry Dump
    if (dump[3] != ORDER_INQUIRY || dump[4] != ORDER_INQUIRY_RESPONSE) return false;

    // Waldorf Music Manufacturer ID
    if (dump[5] != DEVICE_MANUFACTURER_ID) return false;

    // Device Family Code
    if (dump[6] != DEVICE_FAMILY_CODE) return false;

    inquiry_dump.received = true;
    inquiry_dump.device_id = static_cast<int>(dump[2]);
    inquiry_dump.firmware_version = std::format("{0}.{1}", dump[10], dump[11]);

    return true;
}

// DSI: Streichfett
void checkDump(const ByteVec& dump, DumpType type)
{
    if (!isSysex(dump))
    {
        throw std::exception("checkDump failed (not SysEx)");
    }

    // Waldorf Music Manufacturer ID
    if (dump[1] != DEVICE_MANUFACTURER_ID)
    {
        throw std::exception("checkDump failed (SysEx has incorrect manufacture id)");
    }

    //  Device Family Code
    if (dump[2] != DEVICE_FAMILY_CODE)
    {
        throw std::exception("checkDump failed (SysEx has incorrect device family code)");
    }

    if (type == DumpType::Sound)
    {
        if (dump[4] != ORDER_SOUND_DUMP)
        {
            throw std::exception("checkDump failed (not sound dump SysEx)");
        }
    }
    else if (type == DumpType::Global)
    {
        if (dump[4] != ORDER_GLOBAL_DUMP)
        {
            throw std::exception("checkDump failed (not global dump SysEx)");
        }
    }
    else
    {
        throw std::exception("checkDump failed (unknown dump type specified)");
    }
}

const ByteVec getDataBytesFromDump(const ByteVec& dump, DumpType type)
{
    ByteVec data;
    if (type == DumpType::Sound)
    {
        for (auto i = SOUND_DATA_INDEX_FIRST; i <= SOUND_DATA_INDEX_LAST; ++i)
        {
            data.push_back(dump[i]);
        }
    }
    else if (type == DumpType::Global)
    {
        for (auto i = GLOBAL_DATA_INDEX_FIRST; i <= GLOBAL_DATA_INDEX_LAST; ++i)
        {
            data.push_back(dump[i]);
        }
    }
    else
    {
        throw std::exception("getDataBytesFromDump failed (unknown dump type specified)");
    }

    return data;
}

// DSI: Streichfett
const ByteVec getSoundDumpMessageFromPatch(int sound, const SoundModel::Patch& patch)
{
    const auto sound_data = InternalPatch::getDataBytes(patch);
    const auto device_id = InternalSetting::getDeviceId();

    ByteVec req;
    req.clear();
    req.push_back(SYSEX_FIRST);
    req.push_back(DEVICE_MANUFACTURER_ID);
    req.push_back(DEVICE_FAMILY_CODE);
    req.push_back(static_cast<Byte>(device_id));
    req.push_back(ORDER_SOUND_DUMP);
    if (sound == -1)
    {
        req.push_back(static_cast<Byte>(SOUND_EDIT_BUFFER));
    }
    else
    {
        req.push_back(static_cast<Byte>(sound));
    }
    for (auto i = 0; i < sound_data.size(); ++i)
    {
        req.push_back(sound_data[i]);
    }

    // calculate bytesum
    auto sum = 0;
    for (auto i = 1; i < req.size(); ++i)
    {
        sum += req[i];
    }
    req.push_back(static_cast<Byte>(sum & 0x7F));
    req.push_back(SYSEX_LAST);

    return req;
}

// DSI: Streichfett
const ByteVec getSoundParameterChangeMessage(int index, Byte value)
{
    // NOTE: For Streichfett, change the parameters with CC.

    const auto ch = InternalSetting::getDeviceMidiChannel();

    ByteVec req;
    req.clear();
    req.push_back(MIDI_CC + static_cast<Byte>(ch));
    req.push_back(static_cast<Byte>(index));
    req.push_back(value);
    return req;
}

const std::string getByteVecString(const ByteVec& bytes)
{
    std::stringstream ss;

    for (const auto& byte : bytes)
    {
        ss << "0x" << std::uppercase << std::setw(2) << std::hex << static_cast<int>(byte) << " ";
    }

    const auto str = ss.str();

    return str.substr(0, str.size() - 1);
}

#ifdef _DEBUG
const std::string getMessageDesc(const ByteVec& data)
{
    std::stringstream ss;

    if (data.empty())
    {
        ss << "Empty Message";
    }
    else if (MIDI_NOTE_OFF <= data[0] && data[0] <= MIDI_NOTE_ON + MAX_CH_OFFSET)
    {
        if (data[0] < MIDI_NOTE_ON) ss << "Note Off";
        else ss << "Note On";

        ss << " <" << static_cast<int>(data[1]) << "> Vel(" << static_cast<int>(data[2]) << ")";
    }
    else if (MIDI_CC <= data[0] && data[0] <= MIDI_CC + MAX_CH_OFFSET)
    {
        if (data[1] == 0x00)      ss << "Bank Select MSB: " << static_cast<int>(data[2]);
        else if (data[1] == 0x01) ss << "Modulation: " << static_cast<int>(data[2]);
        else if (data[1] == 0x20) ss << "Bank Select LSB: " << static_cast<int>(data[2]);
        else if (data[1] == 0x40) ss << "Damper Pedal: " << static_cast<int>(data[2]);
        else if (data[1] == 0x78) ss << "All Sound Off";
        else if (data[1] == 0x79) ss << "Reset All Controllers";
        else if (data[1] == 0x7A)
        {
            ss << "Local Control";
            if (data[2] == 0)        ss << " Off";
            else if (data[2] == 127) ss << " On";
            else                     ss << " (unknown 3rd byte)";
        }
        else if (data[1] == 0x7B)
        {
            ss << "All Notes Off";
        }
        else if (data[1] == 0x7C)
        {
            ss << "Omni Mode Off";
        }
        else if (data[1] == 0x7D)
        {
            ss << "Omni Mode On";
        }
        else
        {
            ss << "Control Change (" << static_cast<int>(data[1]) << "): "
                << static_cast<int>(data[2]);
        }
    }
    else if (MIDI_PC <= data[0] && data[0] <= MIDI_PC + MAX_CH_OFFSET)
    {
        ss << "Program Change (" << static_cast<int>(data[1]) << ")";
    }
    else if (data[0] == SYSEX_FIRST && data[data.size() - 1] == SYSEX_LAST)
    {
        ss << "SysEx: ";
        if (data[3] == ORDER_INQUIRY)
        {
            if (data[4] == ORDER_INQUIRY_REQUEST)
            {
                ss << "Identity Request";
            }
            else if (data[4] == ORDER_INQUIRY_RESPONSE)
            {
                ss << "Identity Reply";
            }
        }
        else if (data[4] == ORDER_SOUND_REQUEST)
        {
            ss << "Sound Request (";
            if (data[5] == SOUND_EDIT_BUFFER) ss << "edit buffer)";
            else ss << static_cast<int>(data[5]) << ")";
        }
        else if (data[4] == ORDER_GLOBAL_REQUEST)
        {
            ss << "Global Request";
        }
        else if (data[4] == ORDER_SOUND_DUMP)
        {
            ss << "Sound Dump (";
            if (data[5] == SOUND_EDIT_BUFFER) ss << "edit buffer)";
            else ss << static_cast<int>(data[5]) << ")";
        }
        else if (data[4] == ORDER_GLOBAL_DUMP)
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
#endif

} // MessageHandler
} // StreichfettSse
