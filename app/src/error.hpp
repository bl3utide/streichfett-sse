﻿#pragma once
#include "state.hpp"

namespace StreichfettSse
{

extern bool has_error;
extern std::string error_message;
extern bool showing_error_message;

using ErrorWhen = std::uint8_t;
using ErrorCause = std::uint8_t;

// ERROR DEFINITIONS
const ErrorWhen     ERROR_WHEN_INIT             = 0x00;
const ErrorCause    ERROR_CAUSE_INIT_SDL        = 0x00;
const ErrorCause    ERROR_CAUSE_INIT_CONFIG     = 0x01;
const ErrorCause    ERROR_CAUSE_INIT_GUI        = 0x02;
const ErrorCause    ERROR_CAUSE_INIT_IMAGE      = 0x03;
const ErrorCause    ERROR_CAUSE_INIT_CONN       = 0x04;

const ErrorWhen     ERROR_WHEN_FETCH_DEV_LIST   = 0x10;
const ErrorCause    ERROR_CAUSE_GET_INDEV_NAME  = 0x00;
const ErrorCause    ERROR_CAUSE_GET_OUTDEV_NAME = 0x01;

const ErrorWhen     ERROR_WHEN_OPEN_DEV         = 0x20;
const ErrorCause    ERROR_CAUSE_OPEN_DEV_SI     = 0x00;
const ErrorCause    ERROR_CAUSE_OPEN_DEV_SO     = 0x01;
const ErrorCause    ERROR_CAUSE_OPEN_DEV_KI     = 0x02;

const ErrorWhen     ERROR_WHEN_SEND_MESSAGE     = 0x40;
const ErrorCause    ERROR_CAUSE_REQ_INQUIRY     = 0x00;
const ErrorCause    ERROR_CAUSE_REQ_GLOBAL      = 0x01;
const ErrorCause    ERROR_CAUSE_REQ_SOUND       = 0x02;
const ErrorCause    ERROR_CAUSE_SOUND_DUMP      = 0x10;
const ErrorCause    ERROR_CAUSE_PROG_CHANGE     = 0x13;
const ErrorCause    ERROR_CAUSE_ALL_SND_OFF     = 0x14;
const ErrorCause    ERROR_CAUSE_ONE_TASK        = 0x20;

const ErrorWhen     ERROR_WHEN_STATE_PROCESS    = 0xF0;
const ErrorWhen     ERROR_WHEN_RESERVED_FUNC    = 0xF1;

const ErrorWhen     ERROR_WHEN_UNDEFINED        = 0xFF;
const ErrorCause    ERROR_CAUSE_UNDEFINED       = 0xFF;

class AnyCauseException final : public std::exception
{
public:
    explicit AnyCauseException(const char* message, ErrorCause cause = ERROR_CAUSE_UNDEFINED);

    ErrorCause getCause() const noexcept;

    // disable default functions
    AnyCauseException& operator=(const AnyCauseException&) = delete;

private:
    ErrorCause cause_;
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
    BaseException& operator=(const BaseException&) = delete;

private:
    ErrorWhen when_;
    ErrorCause cause_;
    std::string error_message_;
    std::uint16_t getErrorCode() const noexcept;
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

    State getNextState() const noexcept;

    // disable default functions
    ContinuableException& operator=(const ContinuableException&) = delete;

private:
    State next_state_;
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
    UncontinuableException& operator=(const UncontinuableException&) = delete;
};

void initError() noexcept;
void setAppError(const std::string& message) noexcept;

} // StreichfettSse
