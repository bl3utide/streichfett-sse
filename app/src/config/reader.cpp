#include "common.hpp"
#include "logger.hpp"
#include "config/reader.hpp"

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
        Logger::debug(std::format("Loaded config value [{0}]{1}: {2}",
            cv.section_name(),
            cv.key_name(),
            cv.cv()));
    }
    else
    {
        cv.setDefault();
        Logger::debug(std::format("Loaded DEFAULT config value [{0}]{1}: {2}",
            cv.section_name(),
            cv.key_name(),
            cv.cv()));
    }
}

} // Reader
} // Config
} // StreichfettSse
