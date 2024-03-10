#pragma once

namespace StreichfettSse
{
namespace MessageTask
{

//void addParamChangedTask(const int index, const int value); // TODO delete toDvFunc
void addParamChangedTask(const int index, const Byte value);
ByteVec lastTask();
size_t taskSize() noexcept;
#ifdef _DEBUG
size_t largestTaskSizeEver() noexcept;
#endif

} // MessageTask
} // StreichfettSse
