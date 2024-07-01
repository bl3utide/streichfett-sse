#include "common.hpp"
#include "config/section.hpp"

namespace StreichfettSse
{
namespace Config
{

const std::unordered_map<Section, const char*> SECTION_NAMES
{
    { Section::Device,  "device" }
};

const std::unordered_map<Key, const char*> KEY_NAMES
{
    // [device]
    { Key::SynthInputDevice,    "synth_input_device" },
    { Key::SynthOutputDevice,   "synth_output_device" },
    { Key::KeyboardInputDevice, "keyboard_input_device" },
    { Key::ForceAdjustMidiCh,   "force_adjust_midi_ch" },
    { Key::SysExDelay,          "sysex_delay" }
};

} // Config
} // StreichfettSse
