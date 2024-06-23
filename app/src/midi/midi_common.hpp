#pragma once

namespace StreichfettSse
{
namespace Connector
{

struct CallbackMutex
{
    std::mutex guard;
    bool is_callback_catched;

    CallbackMutex()
    {
        is_callback_catched = false;
    }
};

enum class RequestType : int
{
    Confirm = 0,
    GlobalDump,
    SoundDump,
    _COUNT_,
};

extern SDL_TimerID _waiting_timer;
extern bool _is_waiting_store_delay;
extern CallbackMutex callback_mutex;
extern int request_try_count;
extern const int MAX_REQUEST_TRY;
extern const int TIMEOUT_DURATION_PER_RETRY;

bool isWaitingStoreDelay() noexcept;

} // Connector
} // StreichfettSse
