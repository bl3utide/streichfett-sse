#include "common.hpp"
#include "operation.hpp"

namespace StreichfettSse
{

// public
#ifdef _DEBUG
const std::unordered_map<Operation, const char*> OPERATION_STR
{
    { Operation::Sound, "Sound" },
    { Operation::Option, "Option" },
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
