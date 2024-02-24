#pragma once

#include "state.hpp"

namespace StreichfettSse
{
namespace Gui
{

enum class Font : int
{
    Title,
    Version,
    OptionItem,
    OptionItemBold,
    Section,
    Text,
    TextBold,
    Debug,
    DebugProcHead,
    DebugProcHex,
    _COUNT_,
};

void initialize(const char* app_title);
void finalize() noexcept;
void drawGui();

// sub modules
#ifdef _DEBUG
void drawDebugMenuBar(const ImVec2 viewport_pos);
void drawDebugWindows(const int window_w, const int window_h, const State current_state);
#endif

} // Gui
} // StreichfettSse
