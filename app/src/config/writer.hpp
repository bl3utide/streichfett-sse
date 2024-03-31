#pragma once
#include "config/cv.hpp"

namespace StreichfettSse
{
namespace Config
{
namespace Writer
{

void cvToIni(Cv& cv, mINI::INIStructure& is) noexcept;

} // Writer
} // Config
} // StreichfettSse
