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
    Ev(const char* name, int v_min, int v_max, int v_def,
        const std::vector<std::string>* value_arr = nullptr,
        int(* const to_ev_fn)(const int) = nullptr,
        int(* const to_dv_fn)(const int) = nullptr)
        : _name(name), _min(v_min), _max(v_max), _def(v_def),
        _editor_value_arr(value_arr),
        _to_ev_func(to_ev_fn),
        _to_dv_func(to_dv_fn)
    {
        if (_min > _max) throw std::exception("Max-value is less than min-value");

        if (_def < _min) throw std::exception("Default-value is less than min-value");
        if (_def > _max) throw std::exception("Default-value is larger than max-value");

        if (haveEvArr())
        {
            if (_min < 0)
                throw std::exception("The minimum index of Ev string vector must not be negative");
            if (_min != 0)
                throw std::exception("The minimum index of Ev string vector must be 0");
            if (_max > _editor_value_arr->size() - 1)
                throw std::exception("The maximum internal value of Ev must not be larger than the size of the vector<string>");
        }

        if ((_to_ev_func != nullptr && _to_dv_func == nullptr) ||
            (_to_ev_func == nullptr && _to_dv_func != nullptr))
            throw std::exception("Cannot set only to_ev_fn or only to_dv_fn");
        else
        {
            if (_to_ev_func == nullptr)
                _has_converter = false;
            else
                _has_converter = true;
        }

        _editor_value = v_def;
    }

    Ev() = delete;

    Ev& operator=(const Ev& rhs)
    {
        _editor_value = rhs._editor_value;
        return *this;
    }

    Ev& operator=(const int rhs)
    {
        if (_has_converter && isWithinRange(_to_ev_func(rhs)))
        {
            _editor_value = _to_ev_func(rhs);
        }
        else if (!_has_converter && isWithinRange(rhs))
        {
            _editor_value = rhs;
        }
        else throw std::exception("The internal value of Ev is out of range");
        return *this;
    }

    inline bool operator==(const Ev& rhs) const { return _editor_value == rhs._editor_value; }
    inline bool operator==(const int& rhs) const { return _editor_value == rhs; }
    inline bool operator!=(const Ev& rhs) const { return _editor_value != rhs._editor_value; }
    inline bool operator!=(const int& rhs) const { return _editor_value != rhs; }
    inline Ev& operator++() // ++v
    {
        if (isWithinRange(_editor_value + 1)) ++_editor_value;
        return *this;
    }
    inline Ev& operator--() // --v
    {
        if (isWithinRange(_editor_value - 1)) --_editor_value;
        return *this;
    }

    int ev() const
    {
        return _editor_value;
    }

    const char* evs() const
    {
        if (haveEvArr())
            return _editor_value_arr->at(_editor_value).c_str();
        else
            return "(n/a)";
    }

    Byte toDv() const
    {
        if (_has_converter)
            return static_cast<Byte>(_to_dv_func(_editor_value));
        else
            return static_cast<Byte>(_editor_value);
    }

    int max() const { return _max; }
    int min() const { return _min; }
    void setDefault() { _editor_value = _def; }
    //const char* name() const { return _name.c_str(); }
    const std::string& name() const { return _name; }

    bool hasArr() const { return _editor_value_arr != nullptr; }

    size_t evArraySize() const
    {
        return _editor_value_arr != nullptr ? _editor_value_arr->size() : 0;
    }

    const char* getEvArrayItem(const int& i) const
    {
        return _editor_value_arr != nullptr ? _editor_value_arr->at(i).c_str() : "(n/a)";
    }

    int* valp = &_editor_value;

private:
    int _editor_value;
    const int _min, _max, _def;
    const std::vector<std::string>* const _editor_value_arr;
    const std::string _name;
    bool _has_converter;
    int(* const _to_ev_func)(const int);
    int(* const _to_dv_func)(const int);
    bool isWithinRange(const int& v) const { return _min <= v && v <= _max; }
    bool haveEvArr() const { return _editor_value_arr != nullptr; }
};

} // StreichfettSse
