#include "common.hpp"
#include "config/writer.hpp"

namespace StreichfettSse
{
namespace Config
{
namespace Writer
{

void cvToIni(Cv& cv, mINI::INIStructure& is) noexcept
{
    is[cv.section_name()][cv.key_name()] = cv.cv();
#ifdef _DEBUG
    LOGD << "Writed config value [" << cv.section_name() << "]" << cv.key_name() << ": " << cv.cv();
#endif
}

} // Writer
} // Config
} // StreichfettSse
