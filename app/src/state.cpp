#include "common.hpp"
#include "operation.hpp"
#include "state.hpp"
#include "config/config.hpp"
#include "data/internal_patch.hpp"
#include "data/internal_setting.hpp"
#include "midi/connector.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace StreichfettSse
{

// public
#ifdef _DEBUG
const char* STATE_STR[static_cast<int>(State::_COUNT_)] =
{
    "InitInternalData",
    "ApplyConfig",
    "Idle",
    "RequestInquiry",
    "WaitingConfirm",
    "RequestGlobal",
    "WaitingGlobal",
    "SendBankProgChange",
    "RequestSound",
    "WaitingSound",
    "EnterSoundMode",
    "EnterOptionMode",
    "WaitingSendDelay",
    "PrepareToExit",
    "None"
};
#endif

// private
State _state = State::InitInternalData;
State _next_state = State::None;    // the next state that change in the next loop

bool processForCurrentState()
{
    switch (_state)
    {
    case State::InitInternalData:
        InternalPatch::initData();
        InternalSetting::initData();
        Connector::resetAllConnections();
        setNextState(State::ApplyConfig);
        break;
    case State::ApplyConfig:
        Config::load();
        Connector::applyConfig();
        setNextState(State::Idle);
        break;
    case State::Idle:
        Connector::sendOneTaskMessage();
        break;
    case State::RequestInquiry:
        Connector::requestInquiry();
        break;
    case State::RequestGlobal:
        Connector::requestGlobalData();
        break;
    case State::SendBankProgChange:
        Connector::sendProgChange();
        break;
    case State::RequestSound:
        Connector::requestSoundData();
        break;
    case State::EnterSoundMode:
        setOperation(Operation::Sound);
        if (Connector::isSynthConnected()) setNextState(State::RequestGlobal);
        else setNextState(State::Idle);
        break;
    case State::EnterOptionMode:
        setOperation(Operation::Option);
        if (Connector::isSynthConnected()) setNextState(State::RequestGlobal);
        else setNextState(State::Idle);
        break;
    case State::PrepareToExit:
        Connector::updateConfig();
        return false;
        break;
    default:
        break;
    }

    return true;
}

State getState() noexcept
{
    return _state;
}

State getNextState() noexcept
{
    return _next_state;
}

void setNextState(State state, const bool force_mod) noexcept
{
    if (state == State::None)
        return;

    if (_next_state == State::None || force_mod)
    {
        _next_state = state;
#ifdef _DEBUG
        LOGD << "setNextState: [" << static_cast<int>(_next_state) << "]"
            << STATE_STR[static_cast<int>(_next_state)]
            << " (current: " << STATE_STR[static_cast<int>(_state)] << ")";
#endif
    }
    else
    {
#ifdef _DEBUG
        LOGD << "*** called multiple times in one loop ***";
        LOGD << " -> current_state: " << STATE_STR[static_cast<int>(_state)];
        LOGD << " -> next_state:    " << STATE_STR[static_cast<int>(_next_state)];
        LOGD << " -> arg:           " << STATE_STR[static_cast<int>(state)];
#endif
    }
}

void transitionState() noexcept
{
    _state = _next_state;
    _next_state = State::None;
#ifdef _DEBUG
    LOGD << "State changed to [" << static_cast<int>(_state) << "]"
        << STATE_STR[static_cast<int>(_state)];
#endif
}

} // StreichfettSse
