#pragma once

namespace StreichfettSse
{

extern bool has_error;
extern std::string error_message;
extern bool showing_error_message;

void setAppError(const std::string& message) noexcept;

} // StreichfettSse
