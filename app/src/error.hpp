#pragma once
#include "state.hpp"

namespace StreichfettSse
{

extern bool has_error;
extern std::string error_message;
extern bool showing_error_message;

using ErrorWhen = std::uint8_t;
using ErrorCause = std::uint8_t;

// ERROR DEFINITIONS
constexpr ErrorWhen     ERROR_WHEN_UNDEFINED    = 0xFF;
constexpr ErrorCause    ERROR_CAUSE_UNDEFINED   = 0xFF;
constexpr ErrorWhen     ERROR_WHEN_INIT         = 0xF0;
constexpr ErrorCause    ERROR_CAUSE_INIT_SDL    = 0x00;
constexpr ErrorCause    ERROR_CAUSE_INIT_GUI    = 0x01;
constexpr ErrorCause    ERROR_CAUSE_INIT_IMAGE  = 0x02;
constexpr ErrorCause    ERROR_CAUSE_INIT_CONN   = 0x03;
constexpr ErrorCause    ERROR_CAUSE_INIT_CONFIG = 0x04;

class AnyCauseException final : public std::exception
{
public:
    explicit AnyCauseException(const char* message, ErrorCause cause = ERROR_CAUSE_UNDEFINED);

    const ErrorCause getCause() const noexcept;

    // disable default functions
    AnyCauseException(const AnyCauseException&) = delete;
    AnyCauseException& operator=(const AnyCauseException&) = delete;

private:
    ErrorCause _cause;
};

class BaseException : public std::exception
{
public:
    explicit BaseException(
        const AnyCauseException& ace,
        ErrorWhen when = ERROR_WHEN_UNDEFINED
    );

    explicit BaseException(
        const char* message,
        ErrorWhen when = ERROR_WHEN_UNDEFINED,
        ErrorCause cause = ERROR_CAUSE_UNDEFINED
    );

    virtual ~BaseException()
    {
    }

    const std::string& getErrorMessage() const noexcept;

    // disable default functions
    BaseException(const BaseException&) = delete;
    BaseException& operator=(const BaseException&) = delete;

private:
    ErrorWhen _when;
    ErrorCause _cause;
    std::string _error_message;
    const std::uint16_t getErrorCode() const noexcept;
    void setErrorMessage() noexcept;
};

class ContinuableException final : public BaseException
{
public:
    explicit ContinuableException(
        const AnyCauseException& ace,
        ErrorWhen when = ERROR_WHEN_UNDEFINED,
        State next_state = State::None
    );

    explicit ContinuableException(
        const char* message,
        ErrorWhen when = ERROR_WHEN_UNDEFINED,
        ErrorCause cause = ERROR_CAUSE_UNDEFINED,
        State next_state = State::None
    );

    const State getNextState() const noexcept;

    // disable default functions
    ContinuableException(const ContinuableException&) = delete;
    ContinuableException& operator=(const ContinuableException&) = delete;

private:
    State _next_state;
};

class UncontinuableException final : public BaseException
{
public:
    explicit UncontinuableException(
        const AnyCauseException& ace,
        ErrorWhen when = ERROR_WHEN_UNDEFINED
    );

    explicit UncontinuableException(
        const char* message,
        ErrorWhen when = ERROR_WHEN_UNDEFINED,
        ErrorCause cause = ERROR_CAUSE_UNDEFINED
    );

    // disable default functions
    UncontinuableException(const UncontinuableException&) = delete;
    UncontinuableException& operator=(const UncontinuableException&) = delete;
};

void setAppError(const std::string& message) noexcept;

} // StreichfettSse
