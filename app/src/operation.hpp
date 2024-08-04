#pragma once

namespace StreichfettSse
{

enum class Operation : int
{
    Sound,      // request Global Dump
                // -> Program Change (Sound)
                // -> request Sound Dump
                // -> Idle
    Option,     // request Global Dump
                // -> Idle
    _COUNT_,
};

#ifdef _DEBUG
extern const std::unordered_map<Operation, const char*> OPERATION_STR;
#endif

Operation getOperation() noexcept;
void setOperation(Operation operation) noexcept;

} // StreichfettSse
