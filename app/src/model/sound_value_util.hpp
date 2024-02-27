#pragma once

namespace StreichfettSse
{
namespace SoundModel
{

inline int toCenterEv(const int dv) noexcept { return dv - 64; }
inline int centerEvToDv(const int ev) noexcept { return ev + 64; }

} // SoundModel
} // StreichfettSse
