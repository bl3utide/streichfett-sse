#pragma once

namespace StreichfettSse
{
namespace Connector
{

void receiveConfirmSysexCallback(double delta_time, MessageHandler::Bytes* message, void* user_data);
void receiveGlobalDumpSysexCallback(double delta_time, MessageHandler::Bytes* message, void* user_data);
void receiveSoundDumpSysexCallback(double delta_time, MessageHandler::Bytes* message, void* user_data);
Uint32 timeoutCallback(Uint32 interval, void* param);
void receiveKeyDeviceMessageCallback(double delta_time, MessageHandler::Bytes* message, void* user_data);
Uint32 storeDelayCallback(Uint32 interval, void* param);
Uint32 sendDelayCallback(Uint32 interval, void* param);

} // Connector
} // StreichfettSse
