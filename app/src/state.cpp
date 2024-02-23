#include "common.hpp"
#include "state.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

// TODO change app namespace
namespace ImGuiProject
{

// public
#ifdef _DEBUG
const char* STATE_STR[static_cast<int>(State::_COUNT_)] =
{
    "InitInternalData",
    "Idle",
    "None"
};
#endif

// private
State _state = State::InitInternalData;
State _next_state = State::None;    // the next state that change in the next loop

State getState() noexcept
{
    return _state;
}

State getNextState() noexcept
{
    return _next_state;
}

void setNextState(State state) noexcept
{
    _next_state = state;
#ifdef _DEBUG
    LOGD << "setNextState: [" << static_cast<int>(_next_state) << "]"
        << STATE_STR[static_cast<int>(_next_state)];
#endif
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

} // ImGuiProject
