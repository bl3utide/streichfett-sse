#pragma once

namespace StreichfettSse
{
namespace Connector
{
namespace Callback
{

void receiveDeviceInquiryDump(double delta_time, ByteVec* message, void* user_data);
void receiveGlobalDump(double delta_time, ByteVec* message, void* user_data);
void receiveSoundDump(double delta_time, ByteVec* message, void* user_data);
Uint32 timeout(Uint32 interval, void* param);
void receiveKeyDeviceMessage(double delta_time, ByteVec* message, void* user_data);
Uint32 storeDelay(Uint32 interval, void* param);
Uint32 sendDelay(Uint32 interval, void* param);

} // Callback
} // Connector
} // StreichfettSse
