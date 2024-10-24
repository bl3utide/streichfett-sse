﻿#pragma once
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
        Bool,
    };

    // constructor for std::string
    explicit Cv(Section section, Key key, const std::string& v)
        : min_(0), max_(0), def_(0), type_(Type::String)
    {
        setSectionKey(section, key);
        v_ = v;
    }

    // constructor for int
    explicit Cv(Section section, Key key, int v_min, int v_max, int v_def)
        : min_(v_min), max_(v_max), def_(v_def), type_(Type::Int)
    {
        if (min_ > max_) throw std::exception("Max-value is less than min-value");

        if (def_ < min_) throw std::exception("Default-value is less than min-value");
        if (def_ > max_) throw std::exception("Default-value is larger than max-value");

        setSectionKey(section, key);
        v_ = std::to_string(v_def);
    }

    // constructor for bool
    explicit Cv(Section section, Key key, bool v)
        : min_(0), max_(0), def_(0), type_(Type::Bool)
    {
        setSectionKey(section, key);
        v_ = v ? "1" : "0";
    }

    Cv() = delete;

    void set(const std::string& v)
    {
        if (type_ == Type::Int)
        {
            if (isWithinRange(std::atoi(v.c_str())))
            {
                v_ = v;
            }
            else
            {
                throw std::exception(std::format("the internal config value is out of range ({0}::{1})", section_, key_).c_str());
            }
        }
        else if (type_ == Type::Bool)
        {
            v_ = v == "1" ? v : "0";
        }
        else
        {   // Type::String
            v_ = v;
        }
    }

    const std::string& cv() const noexcept { return v_; }
    Type type() const noexcept { return type_; }
    const std::string& type_str() const noexcept { return TYPE_STR.at(type_); }
    int max() const noexcept { return max_; }
    int min() const noexcept { return min_; }

    // works only if type_ == Type::Int
    void setDefault() noexcept {
        if (type_ == Type::Int)
        {
            v_ = std::to_string(def_);
        }
    }

    const std::string& section_name() const noexcept { return section_; }
    const std::string& key_name() const noexcept { return key_; }

private:
    std::string section_;
    std::string key_;
    Type type_;
    std::string v_;
    int min_, max_, def_;   // use if type_ == Type::Int
    static const std::unordered_map<Type, std::string> TYPE_STR;

    void setSectionKey(Section section, Key key)
    {
        section_ = SECTION_NAMES.at(section);
        key_ = KEY_NAMES.at(key);
    }

    bool isWithinRange(int v) const noexcept
    {
        return min_ <= v && v <= max_;
    };
};

} // Config
} // StreichfettSse
