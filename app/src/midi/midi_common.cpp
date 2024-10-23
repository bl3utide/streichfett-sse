#include "common.hpp"
#include "midi/midi_common.hpp"

namespace StreichfettSse
{
namespace Midi
{

// public
DeviceInquiryInfo inquiry_info;
SDL_TimerID waiting_timer;
CallbackMutex callback_mutex;
RequestCounter request_try_count;
const int MAX_REQUEST_TRY = 5;
const int TIMEOUT_DURATION_PER_RETRY = 200;

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
