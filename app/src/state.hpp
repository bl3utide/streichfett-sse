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
extern const char* STATE_STR[static_cast<int>(State::_COUNT_)];
#endif

State getState() noexcept;
State getNextState() noexcept;
void setNextState(State state, const bool force_mod = false) noexcept;
void transitionState() noexcept;

} // StreichfettSse
