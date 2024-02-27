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
Operation _operation = Operation::Sound;

Operation getOperation() noexcept
{
    return _operation;
}

void setOperation(Operation operation) noexcept
{
    _operation = operation;
}

} // StreichfettSse
