#include "common.hpp"
#include "data/internal_patch.hpp"
#include "data/internal_setting.hpp"
#include "midi/message_handler.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif
namespace StreichfettSse
{
namespace MessageHandler
{

// public
InquiryDump inquiry_dump;

// private
const Byte      MESSAGE_INQUIRY = 0x06;
const Byte      MESSAGE_INQUIRY_REQUEST = 0x01;
const Byte      MESSAGE_INQUIRY_RESPONSE = 0x02;
const Byte      MESSAGE_SOUND_REQUEST = 0x00;                       // DSI: Streichfett
const Byte      MESSAGE_GLOBAL_REQUEST = 0x01;                      // DSI: Streichfett
const Byte      MESSAGE_SOUND_DUMP = 0x10;                          // DSI: Streichfett
const Byte      MESSAGE_GLOBAL_DUMP = 0x11;                         // DSI: Streichfett
const Byte      DEVICE_MANUFACTURER_ID = 0x3E;      // Waldorf
const Byte      DEVICE_FAMILY_CODE = 0x19;          // Streichfett  // DSI: Streichfett
const Byte      SOUND_NUMBER_EDIT_BUFFER = 0x7F;                    // DSI: Streichfett
const size_t    INQUIRY_DUMP_SIZE = 13;                             // DSI: Streichfett
const int       SOUND_DATA_START_INDEX = 6;                         // DSI: Streichfett
const int       SOUND_DATA_END_INDEX = 29;                          // DSI: Streichfett
const int       GLOBAL_DATA_START_INDEX = 6;                        // DSI: Streichfett
const int       GLOBAL_DATA_END_INDEX = 13;                         // DSI: Streichfett

bool isSysex(const ByteVec& mb) noexcept
{
    return mb.front() == 0xF0 && mb.back() == 0xF7;
}

const ByteVec getInquiryRequestMessage()
{
    ByteVec req;
    req.clear();
    req.push_back(0xF0);                    // Universal SysEx Header (2 bytes)
    req.push_back(0x7E);
    req.push_back(0x7F);                    // Device ID: omni
    req.push_back(MESSAGE_INQUIRY);         // Identity Request (2 bytes)
    req.push_back(MESSAGE_INQUIRY_REQUEST);
    req.push_back(0xF7);
    return req;
}

// DSI: Streichfett
const ByteVec getSoundRequestMessage(int sound)
{
    ByteVec req;
    req.clear();
    req.push_back(0xF0);
    req.push_back(DEVICE_MANUFACTURER_ID);
    req.push_back(DEVICE_FAMILY_CODE);
    req.push_back(0x7F);    // Device ID: omni
    req.push_back(MESSAGE_SOUND_REQUEST);
    req.push_back(static_cast<Byte>(sound));
    req.push_back(0xF7);
    return req;
}

// DSI: Streichfett
const ByteVec getGlobalRequestMessage()
{
    ByteVec req;
    req.clear();
    req.push_back(0xF0);
    req.push_back(DEVICE_MANUFACTURER_ID);
    req.push_back(DEVICE_FAMILY_CODE);
    req.push_back(0x7F);    // Device ID: omni
    req.push_back(MESSAGE_GLOBAL_REQUEST);
    req.push_back(0x00);    // Reserved
    req.push_back(0xF7);
    return req;
}

const ByteVec getProgChangeMessage(int value)
{
    const int ch = InternalSetting::getDeviceMidiChannel();
    Byte order_byte = 0xC0 + static_cast<Byte>(ch);

    ByteVec pc;
    pc.clear();
    pc.push_back(order_byte);
    pc.push_back(static_cast<Byte>(value));
    return pc;
}

// DSI: Streichfett
const ByteVec getAllSoundOffMessage()
{
    const auto ch = InternalSetting::getDeviceMidiChannel();
    const Byte order_byte = 0xB0 + static_cast<Byte>(ch);

    ByteVec aso;
    aso.clear();
    aso.push_back(order_byte);
    aso.push_back(static_cast<Byte>(0x78));
    return aso;
}

bool isNoteOff(const ByteVec& mb)
{
    return 0x80 <= mb[0] && mb[0] <= 0x8F;
}

bool isNoteOn(const ByteVec& mb)
{
    return 0x90 <= mb[0] && mb[0] <= 0x9F;
}

// DSI: Streichfett
bool checkInquiryDump(const ByteVec& dump)
{
    if (dump.size() != INQUIRY_DUMP_SIZE) return false;

    if (!isSysex(dump)) return false;

    // Universal SysEx Header
    if (dump[1] != 0x7E) return false;

    // Device Inquiry Dump
    if (dump[3] != MESSAGE_INQUIRY || dump[4] != MESSAGE_INQUIRY_RESPONSE) return false;

    // Waldorf Music Manufacturer ID
    if (dump[5] != DEVICE_MANUFACTURER_ID) return false;

    // Device Family Code
    if (dump[6] != DEVICE_FAMILY_CODE) return false;

    inquiry_dump.received = true;
    inquiry_dump.device_id = (int)dump[2];
    inquiry_dump.firmware_version = StringUtil::format("%d.%d", dump[10], dump[11]);

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
        if (dump[4] != MESSAGE_SOUND_DUMP)
        {
            throw std::exception("checkDump failed (not sound dump SysEx)");
        }
    }
    else if (type == DumpType::Global)
    {
        if (dump[4] != MESSAGE_GLOBAL_DUMP)
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
        for (auto i = SOUND_DATA_START_INDEX; i <= SOUND_DATA_END_INDEX; ++i)
            data.push_back(dump[i]);
    }
    else if (type == DumpType::Global)
    {
        for (auto i = GLOBAL_DATA_START_INDEX; i <= GLOBAL_DATA_END_INDEX; ++i)
            data.push_back(dump[i]);
    }
    else
    {
        throw std::exception("getDataByteFromDump failed (unknown dump type specified)");
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
    req.push_back(0xF0);
    req.push_back(DEVICE_MANUFACTURER_ID);
    req.push_back(DEVICE_FAMILY_CODE);
    req.push_back(static_cast<Byte>(device_id));
    req.push_back(MESSAGE_SOUND_DUMP);
    if (sound == -1)
    {
        req.push_back(static_cast<Byte>(SOUND_NUMBER_EDIT_BUFFER));
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
    req.push_back(0xF7);

    return req;
}

// DSI: Streichfett
//ByteVec getSoundParameterChangeMessage(const int index, const int value)    // TODO delete toDvFunc
const ByteVec getSoundParameterChangeMessage(int index, const Byte& value)
{
    // NOTE: For Streichfett, change the parameters with CC.

    const int ch = InternalSetting::getDeviceMidiChannel();

    ByteVec req;
    req.clear();
    req.push_back(0xB0 + static_cast<Byte>(ch));
    req.push_back(static_cast<Byte>(index));
    //req.push_back(static_cast<Byte>(value)); // TODO delete toDvFunc
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
    else if (0x80 <= data[0] && data[0] <= 0x9F)
    {
        if (data[0] < 0x90) ss << "Note Off";
        else ss << "Note On";

        ss << " <" << static_cast<int>(data[1]) << "> Vel(" << static_cast<int>(data[2]) << ")";
    }
    else if (0xB0 <= data[0] && data[0] <= 0xBF)
    {
        if (data[1] == 0x00)      ss << "Bank Select MSB: " << static_cast<int>(data[2]);
        else if (data[1] == 0x20) ss << "Bank Select LSB: " << static_cast<int>(data[2]);
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
        else
        {
            ss << "Control Change (" << static_cast<int>(data[1]) << "): "
                << static_cast<int>(data[2]);
        }
    }
    else if (0xC0 <= data[0] && data[0] <= 0xCF)
    {
        ss << "Program Change (" << static_cast<int>(data[1]) << ")";
    }
    else if (data[0] == 0xF0 && data[data.size() - 1] == 0xF7)
    {
        ss << "SysEx: ";
        if (data[3] == MESSAGE_INQUIRY)
        {
            if (data[4] == MESSAGE_INQUIRY_REQUEST)
            {
                ss << "Identity Request";
            }
            else if (data[4] == MESSAGE_INQUIRY_RESPONSE)
            {
                ss << "Identity Reply";
            }
        }
        else if (data[4] == MESSAGE_SOUND_REQUEST)
        {
            ss << "Sound Request (";
            if (data[5] == 0x7F) ss << "edit buffer)";
            else ss << static_cast<int>(data[5]) << ")";
        }
        else if (data[4] == MESSAGE_GLOBAL_REQUEST)
        {
            ss << "Global Request";
        }
        else if (data[4] == MESSAGE_SOUND_DUMP)
        {
            ss << "Sound Dump (";
            if (data[5] == 0x7F) ss << "edit buffer)";
            else ss << static_cast<int>(data[5]) << ")";
        }
        else if (data[4] == MESSAGE_GLOBAL_DUMP)
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
