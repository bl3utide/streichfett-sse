﻿#include "common.hpp"
#include "midi/erstwhile_message_handler.hpp"

namespace StreichfettSse
{
namespace Midi
{
namespace TaskList
{

// private
std::list<ByteVec> task_list_;
#ifdef _DEBUG
size_t largest_task_size_ever_ = 0;
#endif

static void addTask(const ByteVec& m)
{
    task_list_.push_front(m);
#ifdef _DEBUG
    if (task_list_.size() > largest_task_size_ever_) largest_task_size_ever_ = task_list_.size();
#endif
}

void addParamChangedTask(int index, Byte value)
{
    if (index != -1)
    {
        addTask(ErstwhileMessageHandler::getSoundParameterChangeMessage(index, value));
    }
}

const ByteVec lastTask()
{
    // NOTE: Since you will call pop_back() immediately after this,
    //       you need getting a copy of the element from the list.
    ByteVec lastTask = task_list_.back();
    task_list_.pop_back();
    return lastTask;
}

size_t taskSize() noexcept
{
    return task_list_.size();
}

#ifdef _DEBUG
size_t largestTaskSizeEver() noexcept
{
    return largest_task_size_ever_;
}
#endif

} // TaskList
} // Midi
} // StreichfettSse