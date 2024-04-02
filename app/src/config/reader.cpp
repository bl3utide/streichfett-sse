#include "common.hpp"
#include "config/reader.hpp"

namespace StreichfettSse
{
namespace Config
{
namespace Reader
{

void iniValueToCv(mINI::INIStructure& is, Cv& cv) noexcept
{
    if (is.get(cv.section_name()).has(cv.key_name()))
    {
        const std::string& src_val = is.get(cv.section_name()).get(cv.key_name());

        if (cv.type() == Cv::Type::Int)
        {
            if (StringUtil::isInteger(src_val))
                cv.set(std::to_string(std::stoi(src_val)));
            else
                cv.setDefault();
        }
        else if (cv.type() == Cv::Type::Bool)
        {
            if (src_val == "1")
                cv.set("1");
            else
                cv.set("0");
        }
        else
        {   // Cv::Type::String
            cv.set(src_val);
        }
#ifdef _DEBUG
        LOGD << "Loaded config value [" << cv.section_name() << "]" << cv.key_name() << ": " << cv.cv();
#endif
    }
    else
    {
        cv.setDefault();
#ifdef _DEBUG
        LOGD << "Loaded DEFAULT config value [" << cv.section_name() << "]" << cv.key_name() << ": " << cv.cv();
#endif
    }
}

} // Reader
} // Config
} // StreichfettSse
