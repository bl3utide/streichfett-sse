#include "common.hpp"
#include "error.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace StreichfettSse
{

// public
bool has_error = false;
std::string error_message;
bool showing_error_message = false;

const std::unordered_map<ContinuableException::Cause, std::string> ContinuableException::_message
{
};

ContinuableException::ContinuableException(const Cause cause)
    : exception(_message.at(cause).c_str())
{
}

const std::unordered_map<UncontinuableException::Cause, std::string> UncontinuableException::_message
{
};

UncontinuableException::UncontinuableException(const Cause cause)
    : runtime_error(_message.at(cause).c_str())
{
}

void setAppError(const std::string& message) noexcept
{
    if (!has_error)
    {
        error_message = message;
        has_error = true;
    }
}

} // StreichfettSse
