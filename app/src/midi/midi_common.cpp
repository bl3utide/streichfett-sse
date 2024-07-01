#include "common.hpp"
#include "midi/midi_common.hpp"

namespace StreichfettSse
{
namespace Connector
{

// public
SDL_TimerID waiting_timer;
CallbackMutex callback_mutex;
const int MAX_REQUEST_TRY = 5;
const int TIMEOUT_DURATION_PER_RETRY = 200;
RequestCounter request_try_count(MAX_REQUEST_TRY);

// private
bool _waiting_store_delay = false;    // store delay check

void setWaitingStoreDelay(const bool is_waiting_store_delay) noexcept
{
    _waiting_store_delay = is_waiting_store_delay;
}

bool isWaitingStoreDelay() noexcept
{
    return _waiting_store_delay;
}

} // Connector
} // StreichfettSse
