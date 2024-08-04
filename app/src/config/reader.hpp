#pragma once
#include "config/cv.hpp"

namespace StreichfettSse
{
namespace Config
{
namespace Reader
{

void iniValueToCv(const mINI::INIStructure& is, Cv& cv) noexcept;

} // Reader
} // Config
} // StreichfettSse
