#include "common.hpp"
#include "config/cv.hpp"

namespace StreichfettSse
{
namespace Config
{

const std::unordered_map<Cv::Type, std::string> Cv::type_str_
{
    {Type::String, "string"},
    {Type::Int, "int"},
    {Type::Bool, "bool"}
};

} // Config
} // StreichfettSse
