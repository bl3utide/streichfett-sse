#ifdef _DEBUG
#pragma once

namespace StreichfettSse
{
namespace Connector
{
namespace Debug
{
namespace Callback
{

void receiveTestSysex(double delta_time, ByteVec* message, void* user_data);
Uint32 timeoutTest(Uint32 interval, void* param);

} // Callback
} // Debug
} // Connector
} // StreichfettSse
#endif
