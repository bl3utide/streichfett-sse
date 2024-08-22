#ifdef _DEBUG
#pragma once

namespace StreichfettSse
{
namespace Midi
{
namespace Callback
{
namespace Debug
{

void receiveTestSysex(double delta_time, ByteVec* message, void* user_data);
Uint32 timeoutTest(Uint32 interval, void* param);

} // Debug
} // Callback
} // Midi
} // StreichfettSse
#endif
