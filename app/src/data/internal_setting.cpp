#include "common.hpp"
#include "data/internal_setting.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace StreichfettSse
{
namespace InternalSetting
{

// private
GlobalModel::Global global_data_;

void initData() noexcept
{
}

GlobalModel::Global* getGlobalData() noexcept
{
    return &global_data_;
}

// DSI: Streichfett
void setSettingFromBytes(GlobalModel::Global* global, const ByteVec& data)
{
    using namespace GlobalModel;

    // convert Device values to Editor values
    try
    {
        global->midi_channel        = data[(int)ParamIndex::MidiChannel];
        global->tuning              = data[(int)ParamIndex::Tuning];
        global->transpose           = data[(int)ParamIndex::Transpose];
        global->pitch_bend_range    = data[(int)ParamIndex::PitchBendRange];
        global->device_id           = data[(int)ParamIndex::DeviceId];
    }
    catch (std::exception& error)
    {
        // The received global data seems to be invalid
        throw std::exception(StringUtil::format("setSettingFromBytes failed: %s", error.what()).c_str());
    }
}

// DSI: Streichfett
int getDeviceMidiChannel() noexcept
{
    return global_data_.midi_channel.ev();
}

int getDeviceId() noexcept
{
    return global_data_.device_id.ev();
}

} // InternalSetting
} // StreichfettSse
