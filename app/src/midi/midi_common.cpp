#include "common.hpp"
#include "midi/midi_common.hpp"

namespace StreichfettSse
{
namespace Midi
{

// public
DeviceInquiryResult inquiry_result;
SDL_TimerID waiting_timer;
CallbackMutex callback_mutex;
const int MAX_REQUEST_TRY = 5;  // TODO not public const var
const int TIMEOUT_DURATION_PER_RETRY = 200;
RequestCounter request_try_count(MAX_REQUEST_TRY);

// private
bool waiting_store_delay_;      // store delay check

void setWaitingStoreDelay(bool is_waiting_store_delay) noexcept
{
    waiting_store_delay_ = is_waiting_store_delay;
}

bool isWaitingStoreDelay() noexcept
{
    return waiting_store_delay_;
}

} // Midi
} // StreichfettSse
