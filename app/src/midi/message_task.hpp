#pragma once

namespace StreichfettSse
{
namespace MessageTask
{

void addParamChangedTask(int index, const Byte& value);
const ByteVec lastTask();
size_t taskSize() noexcept;
#ifdef _DEBUG
size_t largestTaskSizeEver() noexcept;
#endif

} // MessageTask
} // StreichfettSse
