#pragma once

namespace StreichfettSse
{
namespace Midi
{

// ------------ common ------------
const Byte DEVICE_ID_OMNI       = 0x7F;

/* MIDI Specification */
// http://midi.teragonaudio.com/tech/midispec.htm
// Channel Voice
const Byte MCV_STB_NOTE_OFF         = 0x80;
const Byte MCV_STB_NOTE_ON          = 0x90;
const Byte MCV_STB_POLY_KEYPRESS    = 0xA0;
const Byte MCV_STB_CONTROL_CHANGE   = 0xB0;
const Byte MCV_STB_PROGRAM_CHANGE   = 0xC0;
const Byte MCV_STB_CH_PRESSURE      = 0xD0;
const Byte MCV_STB_PITCH_WHEEL_CHG  = 0xE0;
const Byte STB_MAX_CH_OFFSET        = 0x0F;
// Channel Mode
const Byte MCM_DTB_ALL_SOUND_OFF    = 0x78;
const Byte MCM_DTB_RESET_ALL_CTRLR  = 0x79;
const Byte MCM_DTB_LOCAL_CTRL       = 0x7A;
const Byte MCM_DTB_ALL_NOTES_OFF    = 0x7B;
const Byte MCM_DTB_OMNI_OFF         = 0x7C;
const Byte MCM_DTB_OMNI_ON          = 0x7D;
const Byte MCM_DTB_MONO_MODE        = 0x7E;
const Byte MCM_DTB_POLY_MODE        = 0x7F;
// System Common
const Byte MSC_STB_SYSEX            = 0xF0;
const Byte MSC_STB_TIMECODE         = 0xF1;
const Byte MSC_STB_SONG_POSITION    = 0xF2;
const Byte MSC_STB_SONG_SELECT      = 0xF3;
const Byte MSC_STB_TUNE_REQUEST     = 0xF6;
const Byte MSC_END_SYSEX            = 0xF7;
const Byte SYSEX_IDNUM_NONREALTIME  = 0x7E;
const Byte SYSEX_IDNUM_REALTIME     = 0x7F;
// System Realtime
const Byte MSR_STB_CLOCK            = 0xF8;
const Byte MSR_STB_TICK             = 0xF9;
const Byte MSR_STB_START            = 0xFA;
const Byte MSR_STB_CONTINUE         = 0xFB;
const Byte MSR_STB_STOP             = 0xFC;
const Byte MSR_STB_ACTIVE_SENSE     = 0xFE;
const Byte MSR_STB_RESET            = 0xFF;

// DSI: Streichfett
// Order
const Byte ORDER_GENERAL_INFO       = 0x06;
const Byte ORDER_INQUIRY_REQUEST    = 0x01;
const Byte ORDER_INQUIRY_RESPONSE   = 0x02;
const Byte ORDER_SOUND_REQUEST      = 0x00;
const Byte ORDER_SOUND_DUMP         = 0x10;
const Byte ORDER_GLOBAL_REQUEST     = 0x01;
const Byte ORDER_GLOBAL_DUMP        = 0x11;
// Parameter
const Byte DEVICE_MANUFACTURER_ID   = 0x3E;     // Waldorf
const Byte DEVICE_FAMILY_CODE       = 0x19;     // Streichfett
const Byte SOUND_EDIT_BUFFER        = 0x7F;
// Data
const size_t INQUIRY_DUMP_SIZE      = 13;
const size_t GLOBAL_DUMP_SIZE       = 16;
const size_t SOUND_DUMP_SIZE        = 32;
const int GLOBAL_DATA_INDEX_FIRST   = 6;
const int GLOBAL_DATA_INDEX_LAST    = 13;
const int SOUND_DATA_INDEX_FIRST    = 6;
const int SOUND_DATA_INDEX_LAST     = 29;

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

    explicit RequestCounter()
        : v_(0), upper_limit_(0)
    {
    }

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
extern const int MAX_REQUEST_TRY;
extern const int TIMEOUT_DURATION_PER_RETRY;

void setWaitingStoreDelay(bool is_waiting_store_delay) noexcept;
bool isWaitingStoreDelay() noexcept;

} // Midi
} // StreichfettSse
