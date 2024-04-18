#include "common.hpp"
#include "config/config.hpp"
#include "config/cv.hpp"
#include "config/reader.hpp"
#include "config/writer.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace StreichfettSse
{
namespace Config
{

// private
std::string _config_file_name;
std::unordered_map<Key, Cv> _cv_by_key;

const Cv& getCv(const Key key) noexcept
{
    return _cv_by_key.at(key);
}

void load() noexcept
{
    mINI::INIStructure read_is;
    mINI::INIFile file = mINI::INIFile(_config_file_name);

    if (file.read(read_is))
    {
#ifdef _DEBUG
        LOGD << "Load config from existing ini file";
#endif
        // ini-file already exists
        for (int key_i = 0; key_i < static_cast<int>(Key::_COUNT_); ++key_i)
        {
            Key key = static_cast<Key>(key_i);
            Reader::iniValueToCv(read_is, _cv_by_key.at(key));
        }
    }
#ifdef _DEBUG
    else
    {
        LOGD << "Ini file does not exists";
    }
#endif
}

void save() noexcept
{
    mINI::INIStructure write_is;
    mINI::INIFile file = mINI::INIFile(_config_file_name);

    for (int key_i = 0; key_i < static_cast<int>(Key::_COUNT_); ++key_i)
    {
        Key key = static_cast<Key>(key_i);
        Writer::cvToIni(_cv_by_key.at(key), write_is);
    }

    if (!file.write(write_is, true))
    {
#ifdef _DEBUG
        LOGD << "Failed to write config file";
#endif
    }
}

const std::string GET_CONFIG_VALUE_TYPE_ERR_TEXT = "Config key '%s' is not %s";

template<typename T>
const T getConfigValue(const Key key)
{
    throw new std::runtime_error("Unexpected type of Cv");
}

template<>
const std::string getConfigValue(const Key key)
{
    Cv& cv = _cv_by_key.at(key);

    if (cv.type() != Cv::Type::String)
        throw new std::runtime_error(StringUtil::format(GET_CONFIG_VALUE_TYPE_ERR_TEXT, cv.key_name(), "string"));

    return cv.cv();
}

template<>
const int getConfigValue(const Key key)
{
    Cv& cv = _cv_by_key.at(key);

    if (cv.type() != Cv::Type::Int)
        throw new std::runtime_error(StringUtil::format(GET_CONFIG_VALUE_TYPE_ERR_TEXT, cv.key_name(), "int"));

    return std::stoi(cv.cv());
}

template<>
const bool getConfigValue(const Key key)
{
    Cv& cv = _cv_by_key.at(key);

    if (cv.type() != Cv::Type::Bool)
        throw new std::runtime_error(StringUtil::format(GET_CONFIG_VALUE_TYPE_ERR_TEXT, cv.key_name(), "bool"));

    return cv.cv() == "1";
}

const std::string SET_CONFIG_VALUE_TYPE_ERR_TEXT = "The type of config key '%s' is not %s";

template<typename T>
void setConfigValue(const Key key, const T value)
{
    throw new std::runtime_error("Unexpected type of value");
}

template<>
void setConfigValue(const Key key, const std::string value)
{
    Cv& cv = _cv_by_key.at(key);

    if (cv.type() != Cv::Type::String)
        throw new std::runtime_error(StringUtil::format(SET_CONFIG_VALUE_TYPE_ERR_TEXT, cv.key_name(), "string"));

    cv.set(value);
}

template<>
void setConfigValue(const Key key, const int value)
{
    Cv& cv = _cv_by_key.at(key);

    if (cv.type() != Cv::Type::Int)
        throw new std::runtime_error(StringUtil::format(SET_CONFIG_VALUE_TYPE_ERR_TEXT, cv.key_name(), "int"));

    cv.set(std::to_string(value));
}

template<>
void setConfigValue(const Key key, const bool value)
{
    Cv& cv = _cv_by_key.at(key);

    if (cv.type() != Cv::Type::Bool)
        throw new std::runtime_error(StringUtil::format(SET_CONFIG_VALUE_TYPE_ERR_TEXT, cv.key_name(), "bool"));

    std::string set_v = value ? "1" : "0";
    cv.set(set_v);
}

void initialize()
{
    _config_file_name = StringUtil::format("%s.ini", APP_NAME.c_str());

    // [Device]
    _cv_by_key.insert({ Key::SynthInputDevice,   Cv(Section::Device, Key::SynthInputDevice, std::string()) });
    _cv_by_key.insert({ Key::SynthOutputDevice,  Cv(Section::Device, Key::SynthOutputDevice, std::string()) });
    _cv_by_key.insert({ Key::KeyboardInputDevice,Cv(Section::Device, Key::KeyboardInputDevice, std::string()) });
    _cv_by_key.insert({ Key::ForceAdjustMidiCh,  Cv(Section::Device, Key::ForceAdjustMidiCh, true) });
    _cv_by_key.insert({ Key::SysExDelay,         Cv(Section::Device, Key::SysExDelay, 200, 500, 200) });
}

} // Config
} // StreichfettSse
