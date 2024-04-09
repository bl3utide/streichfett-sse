#pragma once

namespace StreichfettSse
{

extern bool has_error;
extern std::string error_message;
extern bool showing_error_message;

class ContinuableException : public std::exception
{
public:
    ContinuableException(const char* message);
};

class UncontinuableException : public std::runtime_error
{
public:
    UncontinuableException(const char* message);
};

void setAppError(const std::string& message) noexcept;

} // StreichfettSse
