#pragma once

namespace StreichfettSse
{
namespace GlobalModel
{

inline int toTuningEv(int dv) { return dv - 64; }
inline int TuningEvToDv(int ev) { return ev + 64; }

inline int toTransposeEv(int dv) { return dv - 52; }
inline int TransposeEvToDv(int ev) { return ev + 52; }

} // GlobalModel
} // StreichfettSse

