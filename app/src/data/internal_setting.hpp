#pragma once

#include "model/global.hpp"

namespace StreichfettSse
{
namespace InternalSetting
{

void initData() noexcept;
GlobalModel::Global* getGlobalData() noexcept;
void setSettingFromBytes(GlobalModel::Global* global, const ByteVec& data);
int getDeviceMidiChannel() noexcept;
int getDeviceId() noexcept;

} // InternalSetting
} // StreichfettSse
