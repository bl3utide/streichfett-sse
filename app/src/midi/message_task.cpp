﻿#include "common.hpp"
#include "midi/message_handler.hpp"
#include "midi/message_task.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace StreichfettSse
{
namespace MessageTask
{

// private
std::list<ByteVec> _task_list;
#ifdef _DEBUG
size_t _largest_task_size_ever = 0;
#endif

void addTask(ByteVec& m)
{
    _task_list.push_front(m);
#ifdef _DEBUG
    if (_task_list.size() > _largest_task_size_ever) _largest_task_size_ever = _task_list.size();
#endif
}

//void addParamChangedTask(const int index, const int value)    // TODO delete toDvFunc
void addParamChangedTask(const int index, const Byte value)
{
    if (index != -1)
        addTask(MessageHandler::getSoundParameterChangeMessage(index, value));
}

ByteVec lastTask()
{
    ByteVec lastTask = _task_list.back();
    _task_list.pop_back();
    return lastTask;
}

size_t taskSize() noexcept
{
    return _task_list.size();
}

#ifdef _DEBUG
size_t largestTaskSizeEver() noexcept
{
    return _largest_task_size_ever;
}
#endif

} // MessageTask
} // StreichfettSse