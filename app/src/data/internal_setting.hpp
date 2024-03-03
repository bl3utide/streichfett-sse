#pragma once

#include "midi/message_handler.hpp"
#include "model/global.hpp"

namespace StreichfettSse
{
namespace InternalSetting
{

void initData() noexcept;
GlobalModel::Global* getGlobalData() noexcept;
void setSettingFromBytes(GlobalModel::Global* global, const MessageHandler::Bytes& data);
int getDeviceMidiChannel() noexcept;
int getDeviceId() noexcept;

} // InternalSetting
} // StreichfettSse
