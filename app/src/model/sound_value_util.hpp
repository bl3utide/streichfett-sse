#pragma once

namespace StreichfettSse
{
namespace SoundModel
{

inline int toCenterEv(int dv) noexcept { return dv - 64; }
inline int centerEvToDv(int ev) noexcept { return ev + 64; }

} // SoundModel
} // StreichfettSse
