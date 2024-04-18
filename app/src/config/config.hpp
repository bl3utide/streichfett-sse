#pragma once
#include "config/cv.hpp"

namespace StreichfettSse
{
namespace Config
{

const Cv& getCv(const Key key) noexcept;

template<typename T> const T getConfigValue(const Key key);
template<> const std::string getConfigValue(const Key key);
template<> const int getConfigValue(const Key key);
template<> const bool getConfigValue(const Key key);

template<typename T> void setConfigValue(const Key key, T value);
template<> void setConfigValue(const Key key, std::string value);
template<> void setConfigValue(const Key key, int value);
template<> void setConfigValue(const Key key, bool value);

void load() noexcept;
void save() noexcept;
void initialize();

} // Config
} // StreichfettSse
