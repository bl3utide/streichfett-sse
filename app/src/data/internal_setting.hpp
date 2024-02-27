#pragma once

#include "midi/message_handler.hpp"
#include "model/global.hpp"

namespace StreichfettSse
{
namespace InternalSetting
{

void initData();
GlobalModel::Global* getGlobalData();
void setSettingFromBytes(GlobalModel::Global* global, const MessageHandler::Bytes& data);
int getDeviceMidiChannel();
int getDeviceId();

} // InternalSetting
} // StreichfettSse
