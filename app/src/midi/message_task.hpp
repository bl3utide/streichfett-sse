#pragma once

namespace StreichfettSse
{
namespace Midi
{
namespace MessageTask
{

void addParamChangedTask(int index, Byte value);
const ByteVec lastTask();
size_t taskSize() noexcept;
#ifdef _DEBUG
size_t largestTaskSizeEver() noexcept;
#endif

} // MessageTask
} // Midi
} // StreichfettSse
