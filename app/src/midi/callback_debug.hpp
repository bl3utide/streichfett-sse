#ifdef _DEBUG
#pragma once

namespace StreichfettSse
{
namespace Connector
{
namespace Debug
{

void receiveTestSysexCallback(double delta_time, Bytes* message, void* user_data);
Uint32 timeoutTestCallback(Uint32 interval, void* param);

} // Debug
} // Connector
} // StreichfettSse
#endif
