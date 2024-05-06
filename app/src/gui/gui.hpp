#pragma once

#include "state.hpp"
#include "model/sound.hpp"

namespace StreichfettSse
{
namespace Gui
{

enum class Page : int
{
    Edit,
    Option,
    _COUNT_,
};

extern std::vector<std::function<void()>> reservedFuncs;

void initialize();
void finalize() noexcept;
void drawGui();
void showMessageBox(Uint32 flags, const char* title, const char* message) noexcept;
void doReservedFuncs();
void clearReservedFuncs() noexcept;

// sub modules
void drawEditPanel(SoundModel::Patch* const cp, SoundModel::Patch* const op);
void drawOptionPanel();
#ifdef _DEBUG
namespace Debug
{
void drawDebugMenuBar(const ImVec2 viewport_pos);
void drawDebugWindows(const int window_w, const int window_h, const State current_state);
} // Debug
#endif

} // Gui
} // StreichfettSse
