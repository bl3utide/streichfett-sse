#include "common.hpp"
#include "error.hpp"

namespace StreichfettSse
{

// public
bool has_error;
std::string error_message;
bool showing_error_message;

// private
const std::string MESSAGE_FMT
{ "%s (ERROR CODE 0x%04X)" };
const std::string MESSAGE_FMT_UNDEFINED
{ "<Unexplained Error> (ERROR CODE 0x%04X)" };

const std::unordered_map<ErrorWhen, std::string> MESSAGE
{
    { ERROR_WHEN_INIT,          "Failed to initialize" },
    { ERROR_WHEN_OPEN_DEV,      "Failed to open a device" },
    { ERROR_WHEN_SEND_MESSAGE,  "Failed to send message" },
    { ERROR_WHEN_STATE_PROCESS, "Failed in process for current state"},
    { ERROR_WHEN_RESERVED_FUNC, "Failed in execution reserved function"},
};

AnyCauseException::AnyCauseException(const char* message, ErrorCause cause)
    : std::exception(message), cause_(cause)
{
}

ErrorCause AnyCauseException::getCause() const noexcept
{
    return cause_;
}

BaseException::BaseException(const AnyCauseException& ace, ErrorWhen when)
    : std::exception(ace.what()), when_(when), cause_(ace.getCause())
{
    setErrorMessage();
}

BaseException::BaseException(const char* message, ErrorWhen when, ErrorCause cause)
    : std::exception(message), when_(when), cause_(cause)
{
    setErrorMessage();
}

void BaseException::setErrorMessage() noexcept
{
    if (MESSAGE.find(when_) != MESSAGE.end())
    {
        error_message_ =
            StringUtil::format(MESSAGE_FMT, MESSAGE.at(when_).c_str(), getErrorCode());
    }
    else
    {
        error_message_ =
            StringUtil::format(MESSAGE_FMT_UNDEFINED, getErrorCode());
    }
}

const std::uint16_t BaseException::getErrorCode() const noexcept
{
    return static_cast<std::uint16_t>(when_) << 8 | cause_;
}

const std::string& BaseException::getErrorMessage() const noexcept
{
    return error_message_;
}

ContinuableException::ContinuableException(const AnyCauseException& ace, ErrorWhen when, State next_state)
    : BaseException(ace, when),  next_state_(next_state)
{
}

ContinuableException::ContinuableException(const char* message, ErrorWhen when, ErrorCause cause, State next_state)
    : BaseException(message, when, cause), next_state_(next_state)
{
}

const State ContinuableException::getNextState() const noexcept
{
    return next_state_;
}

UncontinuableException::UncontinuableException(const AnyCauseException& ace, ErrorWhen when)
    : BaseException(ace, when)
{
}

UncontinuableException::UncontinuableException(const char* message, ErrorWhen when, ErrorCause cause)
    : BaseException(message, when, cause)
{
}

void initError() noexcept
{
    has_error = false;
    showing_error_message = false;
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
