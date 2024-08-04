#pragma once

namespace StreichfettSse
{

/*******************************************************************************
    Editor Value Class
*******************************************************************************/
class Ev
{
public:
    /*
    name        parameter name
    v_min       minimum value in editor value
    v_max       maximum value in editor value
    v_def       default value in editor value
                (v_def must be v_min <= v_def <= v_max)
    value_arr   string array for display instead of editor values
                (editor values must start at 0)
    to_ev_fn    function called for conversion when setting editor value from device value
                (called internally only once)
    to_dv_fn    function called when outputting as device value
                (to_ev_fn and to_dv_fn must always be set as a pair)
    */
    explicit Ev(const char* name, int v_min, int v_max, int v_def,
        const std::vector<std::string>* value_arr = nullptr,
        int(* const to_ev_fn)(int) = nullptr,
        int(* const to_dv_fn)(int) = nullptr)
        : name_(name), min_(v_min), max_(v_max), def_(v_def),
        editor_value_arr_(value_arr),
        to_ev_func_(to_ev_fn),
        to_dv_func_(to_dv_fn)
    {
        if (min_ > max_) throw std::exception("Max-value is less than min-value");

        if (def_ < min_) throw std::exception("Default-value is less than min-value");
        if (def_ > max_) throw std::exception("Default-value is larger than max-value");

        if (hasArr())
        {
            if (min_ < 0)
            {
                throw std::exception("The minimum index of Ev string vector must not be negative");
            }
            if (min_ != 0)
            {
                throw std::exception("The minimum index of Ev string vector must be 0");
            }
            if (max_ > editor_value_arr_->size() - 1)
            {
                throw std::exception("The maximum internal value of Ev must not be larger than the size of the vector<string>");
            }
        }

        if ((to_ev_func_ != nullptr && to_dv_func_ == nullptr) ||
            (to_ev_func_ == nullptr && to_dv_func_ != nullptr))
        {
            throw std::exception("Cannot set only to_ev_fn or only to_dv_fn");
        }
        else
        {
            if (to_ev_func_ == nullptr)
            {
                has_converter_ = false;
            }
            else
            {
                has_converter_ = true;
            }
        }

        editor_value_ = v_def;
    }

    Ev() = delete;

    Ev& operator=(const Ev& rhs)
    {
        editor_value_ = rhs.editor_value_;
        return *this;
    }

    Ev& operator=(int rhs)
    {
        if (has_converter_ && isWithinRange(to_ev_func_(rhs)))
        {
            editor_value_ = to_ev_func_(rhs);
        }
        else if (!has_converter_ && isWithinRange(rhs))
        {
            editor_value_ = rhs;
        }
        else
        {
            throw std::exception("The internal value of Ev is out of range");
        }
        return *this;
    }

    inline bool operator==(const Ev& rhs) const { return editor_value_ == rhs.editor_value_; }
    inline bool operator==(int rhs) const { return editor_value_ == rhs; }
    inline bool operator!=(const Ev& rhs) const { return editor_value_ != rhs.editor_value_; }
    inline bool operator!=(int rhs) const { return editor_value_ != rhs; }
    inline Ev& operator++() // ++v
    {
        if (isWithinRange(editor_value_ + 1)) ++editor_value_;
        return *this;
    }
    inline Ev& operator--() // --v
    {
        if (isWithinRange(editor_value_ - 1)) --editor_value_;
        return *this;
    }

    int ev() const
    {
        return editor_value_;
    }

    const char* evs() const
    {
        if (hasArr())
        {
            return editor_value_arr_->at(editor_value_).c_str();
        }
        else
        {
            return "(n/a)";
        }
    }

    Byte toDv() const
    {
        if (has_converter_)
        {
            return static_cast<Byte>(to_dv_func_(editor_value_));
        }
        else
        {
            return static_cast<Byte>(editor_value_);
        }
    }

    int max() const noexcept { return max_; }
    int min() const noexcept { return min_; }
    void setDefault() noexcept { editor_value_ = def_; }
    const std::string& name() const noexcept { return name_; }

    bool hasArr() const noexcept { return editor_value_arr_ != nullptr; }

    size_t evArraySize() const noexcept
    {
        return editor_value_arr_ != nullptr ? editor_value_arr_->size() : 0;
    }

    const char* getEvArrayItem(int i) const noexcept
    {
        return editor_value_arr_ != nullptr ? editor_value_arr_->at(i).c_str() : "(n/a)";
    }

    int* valp = &editor_value_;

private:
    int editor_value_;
    const int min_, max_, def_;
    const std::vector<std::string>* const editor_value_arr_;
    const std::string name_;
    bool has_converter_;
    int(* const to_ev_func_)(int);
    int(* const to_dv_func_)(int);
    bool isWithinRange(int v) const { return min_ <= v && v <= max_; }
};

} // StreichfettSse
