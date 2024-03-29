﻿#include "common.hpp"
#include "data/internal_setting.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace StreichfettSse
{
namespace InternalSetting
{

// private
GlobalModel::Global _global_data;

void initData() noexcept
{
}

GlobalModel::Global* getGlobalData() noexcept
{
    return &_global_data;
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
#ifdef _DEBUG
    catch (std::exception& error)
#else
    catch (std::exception&)
#endif
    {
#ifdef _DEBUG
        LOGD << error.what();
#endif
        throw std::exception("setSettingFromBytes failed");
    }
}

// DSI: Streichfett
int getDeviceMidiChannel() noexcept
{
    return _global_data.midi_channel.ev();
}

int getDeviceId() noexcept
{
    return _global_data.device_id.ev();
}

} // InternalSetting
} // StreichfettSse
