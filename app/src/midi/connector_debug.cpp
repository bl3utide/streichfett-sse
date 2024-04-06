#include "common.hpp"
#ifdef _DEBUG
#include "data/internal_patch.hpp"
#include "midi/midi_common.hpp"
#include "midi/callback_debug.hpp"
#include "midi/connector.hpp"
#include "midi/connector_debug.hpp"
#include "midi/message_handler.hpp"

namespace StreichfettSse
{
namespace Connector
{
namespace Debug
{

// public
SendTestResult send_test[static_cast<int>(SendTestType::_COUNT_)];
SendTestFailedCause send_test_failed_cause[static_cast<int>(SendTestType::_COUNT_)];
std::list<ProcessedMidiMessage> processed_history;
int history_selected_index = -1;
ProcessedMidiMessage selected_processed_message;

// private
const int TIMEOUT_DURATION = 5000;
size_t _processed_history_max_size = 100;

void addProcessedHistory(const bool transmitted, const std::string& device_name, const ByteVec& data)
{
    auto now = std::chrono::system_clock::now();
    auto now_as_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream now_ss;
    now_ss << std::put_time(std::localtime(&now_as_time_t), "%F %T")
        << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
    std::string timestamp = now_ss.str();

    auto description = MessageHandler::getMessageDesc(data);

    processed_history.push_front(ProcessedMidiMessage(timestamp, transmitted, device_name, description, data));

    if (processed_history.size() > _processed_history_max_size)
    {
        processed_history.resize(_processed_history_max_size);
    }

    if (history_selected_index != -1) ++history_selected_index;
}

// DSI: Streichfett
void sendTest(const SendTestType type)
{
    for (int i = 0; i < static_cast<int>(SendTestType::_COUNT_); ++i)
        send_test[i] = SendTestResult::NotStarted;

    ByteVec request;

    switch (type)
    {
        case SendTestType::Inquiry:
            request = MessageHandler::getInquiryRequestMessage();
            break;
        case SendTestType::GlobalDump:
            request = MessageHandler::getGlobalRequestMessage();
            break;
        case SendTestType::SoundDump:
        {
            InternalPatch::SoundAddress* sound_addr = InternalPatch::getCurrentSoundAddress();
            request = MessageHandler::getSoundRequestMessage(sound_addr->sound);
        }
        break;
        default:
            return;
    }

    send_test[static_cast<int>(type)] = SendTestResult::WaitReceive;

    try
    {
        synth_output.sendMessage(&request);
    }
    catch (RtMidiError& error)
    {
        LOGD << error.getMessage();
        send_test[static_cast<int>(type)] = SendTestResult::Failed;
        return;
    }

    SendTestType* type_ptr = new SendTestType(type);

    synth_input.setCallback(Callback::receiveTestSysex, type_ptr);
    synth_input.ignoreTypes(false, false, false);

    // set timer for connection timeout
    _waiting_timer = SDL_AddTimer(TIMEOUT_DURATION, Callback::timeoutTest, type_ptr);

    addProcessedHistory(true, synth_output.getPortName(), request);
}

bool isAnyTestSending() noexcept
{
    return
        send_test[static_cast<int>(SendTestType::Inquiry)] == SendTestResult::WaitReceive ||
        send_test[static_cast<int>(SendTestType::GlobalDump)] == SendTestResult::WaitReceive ||
        send_test[static_cast<int>(SendTestType::SoundDump)] == SendTestResult::WaitReceive;
}

} // Debug
} // Connector
} // StreichfettSse
#endif