#pragma once

namespace StreichfettSse
{
namespace Connector
{
namespace Callback
{

const std::string ERROR_MESSAGE_TIMEOUT[static_cast<int>(RequestType::_COUNT_)]
{
    "The confirm sysex request has timed out.",
    "The global dump request has timed out.",
    "The sound dump request has timed out."
};

void receiveConfirmSysex(double delta_time, ByteVec* message, void* user_data);
void receiveGlobalDumpSysex(double delta_time, ByteVec* message, void* user_data);
void receiveSoundDumpSysex(double delta_time, ByteVec* message, void* user_data);
Uint32 timeout(Uint32 interval, void* param);
void receiveKeyDeviceMessage(double delta_time, ByteVec* message, void* user_data);
Uint32 storeDelay(Uint32 interval, void* param);
Uint32 sendDelay(Uint32 interval, void* param);

} // Callback
} // Connector
} // StreichfettSse
