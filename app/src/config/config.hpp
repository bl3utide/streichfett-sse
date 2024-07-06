#pragma once
#include "config/cv.hpp"

namespace StreichfettSse
{
namespace Config
{

const Cv& getCv(Key key) noexcept;

template<typename T> T getConfigValue(Key key);
template<> std::string getConfigValue(Key key);
template<> int getConfigValue(Key key);
template<> bool getConfigValue(Key key);

template<typename T> void setConfigValue(Key key, T value);
template<> void setConfigValue(Key key, std::string value);
template<> void setConfigValue(Key key, int value);
template<> void setConfigValue(Key key, bool value);

void load() noexcept;
void save() noexcept;
void initialize();

} // Config
} // StreichfettSse
