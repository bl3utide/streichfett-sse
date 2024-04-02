#include "common.hpp"
#include "config/section.hpp"

namespace StreichfettSse
{
namespace Config
{

const char* SECTION_NAMES[static_cast<int>(Section::_COUNT_)]
{
    "device"
};

const char* KEY_NAMES[static_cast<int>(Key::_COUNT_)]
{
    // [device]
    "synth_input_device",
    "synth_output_device",
    "keyboard_input_device",
    "force_adjust_midi_ch",
    "sysex_delay",
};

} // Config
} // StreichfettSse
