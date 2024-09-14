#pragma once
#include "model/ev.hpp"
#include "model/global_const.hpp"
#include "model/global_value_util.hpp"

namespace StreichfettSse
{
namespace GlobalModel
{

// DSI: Streichfett
constexpr auto GPNAME_MIDI_CHANNEL = "MIDI Channel";
constexpr auto GPNAME_TUNING = "Tuning";
constexpr auto GPNAME_TRANSPOSE = "Transpose";
constexpr auto GPNAME_PITCH_BEND_RANGE = "Pitch Bend Range";
constexpr auto GPNAME_DEVICE_ID = "Device ID";

// DSI: Streichfett
enum class ParamIndex : int
{
    MidiChannel = 0,
    Tuning,
    Transpose,
    PitchBendRange,
    DeviceId,
};

// DSI: Streichfett
struct Global
{
    // 0 - 31;  0 - 15: 1 - 16, 16 - 31: receive omni and transmit 17 - 32; default: 16
    Ev midi_channel{ GPNAME_MIDI_CHANNEL, 0, 31, 16, &EV_MIDI_CHANNEL };

    // 0 - 127; -64 - 63;   default: 64;    ev = value - 64
    Ev tuning{ GPNAME_TUNING, 0, 127, 64, nullptr, toTuningEv, TuningEvToDv };

    // 52 - 76; -12 - +12;  default: 64;    ev = value - 52     *** dv_ev_arr_type ***
    Ev transpose{ GPNAME_TRANSPOSE, 0, 24, 12, &EV_TRANSPOSE, toTransposeEv, TransposeEvToDv };

    // 0 - 12;  default: 2
    Ev pitch_bend_range{ GPNAME_PITCH_BEND_RANGE, 0, 12, 2 };

    // 0 - 16;  0(00h) - 126(7Eh)
    Ev device_id{ GPNAME_DEVICE_ID, 0, 126, 0 };

    explicit Global()
    {
        init();
    }

    void init()
    {
        midi_channel.setDefault();
        tuning.setDefault();
        transpose.setDefault();
        pitch_bend_range.setDefault();
        device_id.setDefault();
    }
};

} // GlobalModel
} // StreichfettSse
