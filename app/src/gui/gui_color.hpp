#pragma once

#include "annotation.hpp"

namespace StreichfettSse
{
namespace Gui
{

const ImU32 UI_COLOR_SYMBOL             = IM_COL32(206, 138, 185, 255);     // DSI: Streichfett
const ImU32 UI_COLOR_PARAM_CHANGED      = IM_COL32(255, 117, 214, 255);     // DSI: Streichfett
const ImU32 UI_COLOR_TITLE_TEXT         = UI_COLOR_SYMBOL;
const ImU32 UI_COLOR_VERSION_TEXT       = IM_COL32(160, 160, 160, 255);     // DSI: Streichfett
const ImU32 UI_COLOR_TITLE_BG           = IM_COL32(147, 147, 147, 255);
const ImU32 UI_COLOR_BACKGROUND         = IM_COL32(68, 68, 68, 255);        // DSI: Streichfett
const ImU32 UI_COLOR_BACKGROUND_CHILD   = IM_COL32(50, 50, 50, 255);        // DSI: Streichfett
const ImU32 UI_COLOR_FRAME_BG           = IM_COL32(70, 70, 70, 255);        // DSI: Streichfett
const ImU32 UI_COLOR_FRAME_BG_HOVERED   = IM_COL32(97, 97, 97, 255);        // DSI: Streichfett
const ImU32 UI_COLOR_FRAME_BG_ACTIVE    = IM_COL32(109, 109, 109, 255);     // DSI: Streichfett
const ImU32 UI_COLOR_POPUP_BG           = IM_COL32(72, 72, 72, 255);        // DSI: Streichfett
const ImU32 UI_COLOR_SLIDER_GRAB        = IM_COL32(130, 70, 117, 255);      // DSI: Streichfett
const ImU32 UI_COLOR_SLIDER_GRAB_ACTIVE = IM_COL32(167, 42, 139, 255);      // DSI: Streichfett
const ImU32 UI_COLOR_DMP_BTN            = IM_COL32(111, 40, 90, 255);       // DSI: Streichfett
const ImU32 UI_COLOR_DMP_BTN_HOVERED    = IM_COL32(135, 48, 109, 255);      // DSI: Streichfett
const ImU32 UI_COLOR_DMP_BTN_ACTIVE     = IM_COL32(135, 64, 114, 255);      // DSI: Streichfett
const ImU32 UI_COLOR_HEADER             = IM_COL32(95, 95, 95, 255);        // DSI: Streichfett
const ImU32 UI_COLOR_BORDER             = IM_COL32(128, 128, 128, 255);
const ImU32 UI_COLOR_TEXT_BASE          = IM_COL32(246, 246, 246, 255);     // DSI: Streichfett
const ImU32 UI_COLOR_TEXT_DEV_USED      = IM_COL32(237, 184, 207, 255);     // DSI: Streichfett
const ImU32 UI_COLOR_TEXT_PAGE_ACTIVE   = IM_COL32(219, 219, 219, 255);     // DSI: Streichfett
const ImU32 UI_COLOR_TEXT_PAGE_INACTIVE = IM_COL32(122, 122, 122, 255);     // DSI: Streichfett
const ImU32 UI_COLOR_TEXT_PATCH_INFO    = IM_COL32(197, 197, 197, 255);     // DSI: Streichfett
const ImU32 UI_COLOR_TEXT_CONNECTED     = IM_COL32(110, 206, 107, 255);     // DSI: Streichfett
const ImU32 UI_COLOR_TEXT_DISCONNECTED  = IM_COL32(206, 107, 107, 255);     // DSI: Streichfett
const ImU32 UI_COLOR_ANNOTATION[static_cast<int>(Annotation::Type::_COUNT_)] =
{
    IM_COL32(128, 249, 105, 255),   // DSI: Streichfett
    IM_COL32(249, 169, 105, 255)    // DSI: Streichfett
};
#ifdef _DEBUG
namespace Debug
{
const ImU32 DEBUG_UI_COLOR_TEXT_OK          = IM_COL32(0, 255, 0, 255);
const ImU32 DEBUG_UI_COLOR_TEXT_NG          = IM_COL32(255, 0, 0, 255);
const ImU32 DEBUG_UI_COLOR_TEXT_TRANSMIT    = IM_COL32(120, 185, 241, 255);
const ImU32 DEBUG_UI_COLOR_TEXT_RECEIVE     = IM_COL32(241, 120, 178, 255);
} // Debug
#endif

} // Gui
} // StreichfettSse
