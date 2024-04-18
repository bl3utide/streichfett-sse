#pragma once

#include <stdexcept>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <sstream>
#include <regex>
#include <chrono>
#include <ctime>

#include <SDL.h>
#include <SDL_opengl.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl2.h>

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Appenders/IAppender.h>

#include <mini/ini.h>

#include <RtMidi.h>

#include "util/gui_util.hpp"
#include "util/string_util.hpp"

namespace StreichfettSse
{

#define DEF_APP_NAME        "StreichfettSse"
#define DEF_APP_VERSION     "1.2"
#define DEF_APP_DEV_BY      "bl3utide"
#define DEF_APP_DEV_YR      2024
#define DEF_APP_TITLE       "Streichfett SSE"
#define LDEBUG LOGD
#define LERROR LOGE_(1)

const std::string APP_NAME = DEF_APP_NAME;
using Byte = unsigned char;
using ByteVec = std::vector<Byte>;

} // StreichfettSse
