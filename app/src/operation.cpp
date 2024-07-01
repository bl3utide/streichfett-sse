#include "common.hpp"
#include "operation.hpp"

namespace StreichfettSse
{

// public
#ifdef _DEBUG
const char* OPERATION_STR[static_cast<int>(Operation::_COUNT_)] =
{
    "Sound",
    "Option"
};
#endif

// private
Operation operation_ = Operation::Sound;

Operation getOperation() noexcept
{
    return operation_;
}

void setOperation(Operation operation) noexcept
{
    operation_ = operation;
}

} // StreichfettSse
