#include "common.hpp"
#include "data/local_patch.hpp"
#include "data/local_setting.hpp"
#include "midi/midi_common.hpp"
#include "midi/message_creator/sysex_message_creator.h"

namespace StreichfettSse
{
namespace Midi
{

const ByteVec RequestDeviceInquiryCreator::create() const
{
    ByteVec m;
    m.push_back(SYSEX_FIRST);
    m.push_back(SYSEX_SECOND_UNRT);
    m.push_back(DEVICE_ID_OMNI);
    // Identity Request (2 bytes)
    m.push_back(ORDER_INQUIRY);
    m.push_back(ORDER_INQUIRY_REQUEST);
    m.push_back(SYSEX_LAST);
    return m;
}

// DSI: Streichfett
const ByteVec RequestGlobalCreator::create() const
{
    ByteVec m;
    m.push_back(SYSEX_FIRST);
    m.push_back(DEVICE_MANUFACTURER_ID);
    m.push_back(DEVICE_FAMILY_CODE);
    m.push_back(DEVICE_ID_OMNI);
    m.push_back(ORDER_GLOBAL_REQUEST);
    m.push_back(0x00);    // Reserved
    m.push_back(SYSEX_LAST);
    return m;
}

// DSI: Streichfett
const ByteVec RequestSoundCreator::create() const
{
    ByteVec m;
    m.push_back(SYSEX_FIRST);
    m.push_back(DEVICE_MANUFACTURER_ID);
    m.push_back(DEVICE_FAMILY_CODE);
    m.push_back(DEVICE_ID_OMNI);
    m.push_back(ORDER_SOUND_REQUEST);
    m.push_back(static_cast<Byte>(sound_));
    m.push_back(SYSEX_LAST);
    return m;
}

const ByteVec SoundDumpCreator::create() const
{
    const auto sound_data = LocalPatch::getDataBytes(patch_);
    const auto device_id = LocalSetting::getDeviceId();

    ByteVec m;
    m.push_back(SYSEX_FIRST);
    m.push_back(DEVICE_MANUFACTURER_ID);
    m.push_back(DEVICE_FAMILY_CODE);
    m.push_back(static_cast<Byte>(device_id));
    m.push_back(ORDER_SOUND_DUMP);

    if (sound_ == -1)
    {
        m.push_back(static_cast<Byte>(SOUND_EDIT_BUFFER));
    }
    else
    {
        m.push_back(static_cast<Byte>(sound_));
    }

    for (auto i = 0; i < sound_data.size(); ++i)
    {
        m.push_back(sound_data[i]);
    }

    // calculate bytesum
    auto sum = 0;
    for (auto i = 1; i < m.size(); ++i)
    {
        sum += m[i];
    }
    m.push_back(static_cast<Byte>(sum & 0x7F));
    m.push_back(SYSEX_LAST);

    return m;
}

} // Midi
} // StreichfettSse
