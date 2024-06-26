﻿#pragma once

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

#ifdef _DEBUG
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Appenders/IAppender.h>
#endif

#include <mini/ini.h>

#include <RtMidi.h>

#include "util/string_util.hpp"

// TODO define in external module
namespace StreichfettSse
{
using Byte = unsigned char;
using ByteVec = std::vector<Byte>;
} // StreichfettSse
