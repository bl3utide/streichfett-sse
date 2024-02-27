#pragma once

namespace StreichfettSse
{

enum class State : int
{
    InitInternalData,
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
    None,
    _COUNT_
};

#ifdef _DEBUG
extern const char* STATE_STR[static_cast<int>(State::_COUNT_)];
#endif

State getState() noexcept;
State getNextState() noexcept;
void setNextState(State state) noexcept;
void transitionState() noexcept;

} // StreichfettSse
