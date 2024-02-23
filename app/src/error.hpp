#pragma once

// TODO change app namespace
namespace ImGuiProject
{

extern bool has_error;
extern std::string error_message;
extern bool showing_error_message;

void setAppError(const std::string& message) noexcept;

} // ImGuiProject
