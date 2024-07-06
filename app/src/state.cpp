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
const std::unordered_map<State, const char*> STATE_STR
{
    { State::InitInternalData,      "InitInternalData" },
    { State::ApplyConfig,           "ApplyConfig" },
    { State::Idle,                  "Idle" },
    { State::RequestInquiry,        "RequestInquiry" },
    { State::WaitingConfirm,        "WaitingConfirm" },
    { State::RequestGlobal,         "RequestGlobal" },
    { State::WaitingGlobal,         "WaitingGlobal" },
    { State::SendBankProgChange,    "SendBankProgChange" },
    { State::RequestSound,          "RequestSound" },
    { State::WaitingSound,          "WaitingSound" },
    { State::EnterSoundMode,        "EnterSoundMode" },
    { State::EnterOptionMode,       "EnterOptionMode" },
    { State::WaitingSendDelay,      "WaitingSendDelay" },
    { State::PrepareToExit,         "PrepareToExit" },
    { State::None,                  "None" },
};
#endif

// private
State state_ = State::InitInternalData;
State next_state_ = State::None;    // the next state that change in the next loop

bool processForCurrentState()
{
    switch (state_)
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
    return state_;
}

State getNextState() noexcept
{
    return next_state_;
}

void setNextState(State target_state, bool force_mod) noexcept
{
    if (target_state == State::None)
        return;

    if (next_state_ == State::None || force_mod)
    {
        next_state_ = target_state;
#ifdef _DEBUG
        LDEBUG << "setNextState: [" << static_cast<int>(next_state_) << "]"
            << STATE_STR.at(next_state_)
            << " (current: " << STATE_STR.at(state_) << ")";
#endif
    }
    else
    {
#ifdef _DEBUG
        LDEBUG << "*** called multiple times in one loop ***";
        LDEBUG << " -> current_state: " << STATE_STR.at(state_);
        LDEBUG << " -> next_state:    " << STATE_STR.at(next_state_);
        LDEBUG << " -> arg:           " << STATE_STR.at(target_state);
#endif
    }
}

void transitionState() noexcept
{
    state_ = next_state_;
    next_state_ = State::None;
#ifdef _DEBUG
    LDEBUG << "State changed to [" << static_cast<int>(state_) << "]"
        << STATE_STR.at(state_);
#endif
}

} // StreichfettSse
