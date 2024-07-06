#include "common.hpp"
#include "logger.hpp"
#include "config/writer.hpp"

namespace StreichfettSse
{
namespace Config
{
namespace Writer
{

void cvToIni(const Cv& cv, mINI::INIStructure& is) noexcept
{
    is[cv.section_name()][cv.key_name()] = cv.cv();
    Logger::debug(StringUtil::format("Writed config value [%s]%s: %s",
        cv.section_name().c_str(),
        cv.key_name().c_str(),
        cv.cv().c_str()));
}

} // Writer
} // Config
} // StreichfettSse
