#pragma once

namespace StreichfettSse
{
namespace GlobalModel
{

inline int toTuningEv(const int dv) { return dv - 64; }
inline int TuningEvToDv(const int ev) { return ev + 64; }

inline int toTransposeEv(const int dv) { return dv - 52; }
inline int TransposeEvToDv(const int ev) { return ev + 52; }

} // GlobalModel
} // StreichfettSse

