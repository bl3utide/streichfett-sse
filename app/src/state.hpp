#pragma once

namespace StreichfettSse
{

enum class State : int
{
    InitInternalData,
    ApplyConfig,
    Idle,
    RequestInquiry,
    WaitingConfirm,
    RequestGlobal,
    WaitingGlobal,
    SendBankProgChange,
    RequestSound,
    WaitingSound,
    EnterSoundMode,
    EnterOptionMode,
    WaitingSendDelay,
    PrepareToExit,
    None,
    _COUNT_
};

#ifdef _DEBUG
extern const std::unordered_map<State, const char*> STATE_STR;
#endif

bool processForCurrentState();
State getState() noexcept;
State getNextState() noexcept;
void setNextState(State state, bool force_mod = false) noexcept;
void transitionState() noexcept;

} // StreichfettSse
