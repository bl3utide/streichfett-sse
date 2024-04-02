#pragma once

namespace StreichfettSse
{
namespace Config
{

enum class Section : int
{
    Device,
    _COUNT_,
};

enum class Key : int
{
    // [Device]
    SynthInputDevice,
    SynthOutputDevice,
    KeyboardInputDevice,
    ForceAdjustMidiCh,
    SysExDelay,

    _COUNT_,
};

extern const char* SECTION_NAMES[static_cast<int>(Section::_COUNT_)];
extern const char* KEY_NAMES[static_cast<int>(Key::_COUNT_)];

} // Config
} // StreichfettSse
