#pragma once

namespace StreichfettSse
{
namespace Midi
{

// ------------ common ------------
const Byte SYSEX_FIRST          = 0xF0;
const Byte SYSEX_LAST           = 0xF7;
const Byte SYSEX_SECOND_UNRT    = 0x7E;
const Byte DEVICE_ID_OMNI       = 0x7F;
const Byte MIDI_NOTE_OFF        = 0x80;
const Byte MIDI_NOTE_ON         = 0x90;
const Byte MIDI_CC              = 0xB0;
const Byte MIDI_PC              = 0xC0;
const Byte MAX_CH_OFFSET        = 0x0F;
// ------------ order ------------
// DSI: Streichfett
const Byte ORDER_INQUIRY            = 0x06;
const Byte ORDER_INQUIRY_REQUEST    = 0x01;
const Byte ORDER_INQUIRY_RESPONSE   = 0x02;
const Byte ORDER_SOUND_REQUEST      = 0x00;
const Byte ORDER_SOUND_DUMP         = 0x10;
const Byte ORDER_GLOBAL_REQUEST     = 0x01;
const Byte ORDER_GLOBAL_DUMP        = 0x11;
// ------------ parameter ------------
// DSI: Streichfett
const Byte DEVICE_MANUFACTURER_ID   = 0x3E;     // Waldorf
const Byte DEVICE_FAMILY_CODE       = 0x19;     // Streichfett
const Byte SOUND_EDIT_BUFFER        = 0x7F;
// ------------ data ------------
// DSI: Streichfett
const size_t INQUIRY_DUMP_SIZE      = 13;
const int SOUND_DATA_INDEX_FIRST    = 6;
const int SOUND_DATA_INDEX_LAST     = 29;
const int GLOBAL_DATA_INDEX_FIRST   = 6;
const int GLOBAL_DATA_INDEX_LAST    = 13;

// DSI: Streichfett
class DeviceInquiryResult
{
public:
    explicit DeviceInquiryResult(int device_id, const std::string_view ver)
        : received_(true), device_id_(device_id), firmware_version_(ver)
    {
    }

    explicit DeviceInquiryResult()
        : received_(false), device_id_(-1), firmware_version_("")
    {
    }

    DeviceInquiryResult& operator=(const DeviceInquiryResult& rhs)
    {
        received_ = rhs.received_;
        device_id_ = rhs.device_id_;
        firmware_version_ = rhs.firmware_version_;
        return *this;
    }

    DeviceInquiryResult(const DeviceInquiryResult& rhs) = delete;

    void initialize() noexcept
    {
        received_ = false;
    }

    bool isReceived() const noexcept { return received_; }
    int getDeviceId() const noexcept { return device_id_; }
    const std::string getFirmwareVersion() const noexcept { return firmware_version_; }

private:
    bool received_;
    int device_id_;
    std::string firmware_version_;
};

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

extern DeviceInquiryResult inquiry_result;
extern SDL_TimerID waiting_timer;
extern CallbackMutex callback_mutex;
extern RequestCounter request_try_count;
extern const int TIMEOUT_DURATION_PER_RETRY;

void setWaitingStoreDelay(bool is_waiting_store_delay) noexcept;
bool isWaitingStoreDelay() noexcept;

} // Midi
} // StreichfettSse
