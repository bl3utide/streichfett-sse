#include "common.hpp"
#include "midi/midi_common.hpp"

namespace StreichfettSse
{
namespace Connector
{

// public
SDL_TimerID _waiting_timer;
bool _is_waiting_store_delay = false;   // store delay check

bool isWaitingStoreDelay() noexcept
{
    return _is_waiting_store_delay;
}

} // Connector
} // StreichfettSse
