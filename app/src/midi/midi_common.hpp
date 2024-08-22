#pragma once

namespace StreichfettSse
{
namespace Connector
{

struct CallbackMutex
{
    std::mutex guard;
    bool is_callback_catched;

    explicit CallbackMutex() : is_callback_catched(false)
    {
    }
};

class RequestCounter
{
public:
    explicit RequestCounter(int limit)
        : v_(0), upper_limit_(limit)
    {
    }

    RequestCounter() = delete;
    RequestCounter(const RequestCounter& rhs) = delete;

    void reset() noexcept
    {
        v_ = 0;
    }

    RequestCounter& operator++() noexcept
    {
        ++v_;
        return *this;
    }

    void inc() noexcept
    {
        ++v_;
    }

    bool reachedLimit() const noexcept
    {
        return v_ >= upper_limit_;
    }

    int v() const noexcept
    {
        return v_;
    }

    int max() const noexcept
    {
        return upper_limit_;
    }

private:
    int v_;
    int upper_limit_;
};

enum class RequestType : int
{
    DeviceInquiry = 0,
    Global,
    Sound,
    _COUNT_,
};

extern SDL_TimerID waiting_timer;
extern CallbackMutex callback_mutex;
extern RequestCounter request_try_count;
extern const int TIMEOUT_DURATION_PER_RETRY;

void setWaitingStoreDelay(bool is_waiting_store_delay) noexcept;
bool isWaitingStoreDelay() noexcept;

} // Connector
} // StreichfettSse
