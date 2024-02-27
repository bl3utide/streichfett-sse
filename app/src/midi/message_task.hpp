#pragma once

#include "message_handler.hpp"

namespace StreichfettSse
{
namespace MessageTask
{

//void addParamChangedTask(const int index, const int value); // TODO delete toDvFunc
void addParamChangedTask(const int index, const unsigned char value);
MessageHandler::Bytes lastTask();
size_t taskSize() noexcept;
#ifdef _DEBUG
size_t largestTaskSizeEver() noexcept;
#endif

} // MessageTask
} // StreichfettSse
