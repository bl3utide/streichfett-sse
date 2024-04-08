#include "common.hpp"
#include "except.hpp"

namespace StreichfettSse
{

ContinuableException::ContinuableException(const char* message)
    : exception(message)
{
}

UncontinuableException::UncontinuableException(const char * message)
    : runtime_error(message)
{
}

} // StreichfettSse
