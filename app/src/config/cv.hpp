#pragma once
#include "config/section.hpp"

namespace StreichfettSse
{
namespace Config
{

/*
    Configuration Section-Key-Value Class

    compatible types(expression in .ini)
    - bool          ("1" or "0")
    - int           ("1", "-10")
    - std::string   (same)
*/
class Cv
{
public:
    enum class Type : int
    {
        String,
        Int,
        Bool
    };

    // constructor for std::string
    Cv(const Section section, const Key key, const std::string& v)
        : _min(0), _max(0), _def(0), _type(Type::String)
    {
        setSectionKey(section, key);
        _v = v;
    }

    // constructor for int
    Cv(const Section section, const Key key, const int v_min, const int v_max, const int v_def)
        : _min(v_min), _max(v_max), _def(v_def), _type(Type::Int)
    {
        if (_min > _max) throw std::exception("Max-value is less than min-value");

        if (_def < _min) throw std::exception("Default-value is less than min-value");
        if (_def > _max) throw std::exception("Default-value is larger than max-value");

        setSectionKey(section, key);
        _v = std::to_string(v_def);
    }

    // constructor for bool
    Cv(const Section section, const Key key, const bool v)
        : _min(0), _max(0), _def(0), _type(Type::Bool)
    {
        setSectionKey(section, key);
        _v = v ? "1" : "0";
    }

    Cv() = delete;

    void set(const std::string& v)
    {
        if (_type == Type::Int)
        {
            if (isWithinRange(std::atoi(v.c_str())))
            {
                _v = v;
            }
            else
            {
                throw std::exception(StringUtil::format("the internal config value is out of range (%s::%s)", _section.c_str(), _key.c_str()).c_str());
            }
        }
        else if (_type == Type::Bool)
        {
            _v = v == "1" ? v : "0";
        }
        else
        {   // Type::String
            _v = v;
        }
    }

    const std::string& cv() const noexcept { return _v; }
    const Type& type() const noexcept { return _type; }
    const std::string& type_str() const noexcept { return _type_str.at(_type); }
    int max() const noexcept { return _max; }
    int min() const noexcept { return _min; }

    // works only if _type == Type::Int
    void setDefault() noexcept {
        if (_type == Type::Int)
        {
            _v = std::to_string(_def);
        }
    }

    const std::string& section_name() const noexcept { return _section; }
    const std::string& key_name() const noexcept { return _key; }

private:
    std::string _section;
    std::string _key;
    Type _type;
    std::string _v;
    int _min, _max, _def;   // use if _type == Type::Int
    static const std::unordered_map<Type, std::string> _type_str;

    void setSectionKey(const Section section, const Key key)
    {
        _section = SECTION_NAMES[static_cast<int>(section)];
        _key = KEY_NAMES[static_cast<int>(key)];
    }

    bool isWithinRange(const int v) const noexcept
    {
        return _min <= v && v <= _max;
    };
};

} // Config
} // StreichfettSse
