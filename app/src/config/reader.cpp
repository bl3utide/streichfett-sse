#include "common.hpp"
#include "logger.hpp"
#include "config/cv.hpp"

namespace StreichfettSse
{
namespace Config
{
namespace Reader
{

void iniValueToCv(const mINI::INIStructure& is, Cv& cv) noexcept
{
    if (is.get(cv.section_name()).has(cv.key_name()))
    {
        const auto src_val = is.get(cv.section_name()).get(cv.key_name());

        if (cv.type() == Cv::Type::Int)
        {
            if (StringUtil::isInteger(src_val))
            {
                cv.set(std::to_string(std::stoi(src_val)));
            }
            else
            {
                cv.setDefault();
            }
        }
        else if (cv.type() == Cv::Type::Bool)
        {
            if (src_val == "1") cv.set("1");
            else cv.set("0");
        }
        else
        {   // Cv::Type::String
            cv.set(src_val);
        }
        Logger::debug(StringUtil::format("Loaded config value [%s]%s: %s",
            cv.section_name().c_str(),
            cv.key_name().c_str(),
            cv.cv().c_str()));
    }
    else
    {
        cv.setDefault();
        Logger::debug(StringUtil::format("Loaded DEFAULT config value [%s]%s: %s",
            cv.section_name().c_str(),
            cv.key_name().c_str(),
            cv.cv().c_str()));
    }
}

} // Reader
} // Config
} // StreichfettSse
