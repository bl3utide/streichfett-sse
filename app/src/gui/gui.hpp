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

void initialize(const std::string& title, const std::string& version, const std::string& copyright);
void finalize() noexcept;
void drawGui();

// sub modules
void drawEditPanel(SoundModel::Patch* const cp, SoundModel::Patch* const op);
void drawOptionPanel();
#ifdef _DEBUG
void drawDebugMenuBar(const ImVec2 viewport_pos);
void drawDebugWindows(const int window_w, const int window_h, const State current_state);
bool isChildBgAlt() noexcept;
#endif

} // Gui
} // StreichfettSse
