#include "common.hpp"
#include "midi/midi_common.hpp"

namespace StreichfettSse
{
namespace Connector
{

// public
SDL_TimerID _waiting_timer;
bool _is_waiting_store_delay = false;   // store delay check
CallbackMutex callback_mutex;
const int MAX_REQUEST_TRY = 5;
const int TIMEOUT_DURATION_PER_RETRY = 200;
RequestCounter request_try_count(MAX_REQUEST_TRY);

bool isWaitingStoreDelay() noexcept
{
    return _is_waiting_store_delay;
}

} // Connector
} // StreichfettSse
