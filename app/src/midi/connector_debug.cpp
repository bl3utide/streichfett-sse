﻿#include "common.hpp"
#ifdef _DEBUG
#include "logger.hpp"
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
std::unordered_map<SendTestType, SendTestResult> send_test;
std::unordered_map<SendTestType, SendTestFailedCause> send_test_failed_cause;
std::vector<ProcessedMidiMessage> history;
int history_selected_index;
ProcessedMidiMessage history_selected;
std::mutex history_mutex;

// private
const int TIMEOUT_DURATION = 5000;
const size_t MAX_SIZE_DISPLAY_HISTORY = 200;

void addProcessedHistory(bool transmitted, const std::string& device_name, const ByteVec& data)
{
    auto now = std::chrono::system_clock::now();
    auto now_as_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream now_ss;
    now_ss << std::put_time(std::localtime(&now_as_time_t), "%F %T")
        << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
    const auto timestamp = now_ss.str();

    const auto description = MessageHandler::getMessageDesc(data);

    std::unique_lock lock(history_mutex);
    history.emplace_back(ProcessedMidiMessage(timestamp, transmitted, device_name, description, data));

    if (history.size() > MAX_SIZE_DISPLAY_HISTORY)
    {
        history.erase(history.cbegin());
        --history_selected_index;
    }
}

// DSI: Streichfett
void sendTest(SendTestType type)
{
    for (auto i = 0; i < static_cast<int>(SendTestType::_COUNT_); ++i)
    {
        send_test.at(static_cast<SendTestType>(i)) = SendTestResult::NotStarted;
    }

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
            auto& sound_addr = InternalPatch::getCurrentSoundAddress();
            request = MessageHandler::getSoundRequestMessage(sound_addr.sound);
        }
            break;
        default:
            return;
    }

    send_test.at(type) = SendTestResult::WaitReceive;

    try
    {
        synth_output.sendMessage(request);
    }
    catch (RtMidiError& error)
    {
        Logger::debug(error.getMessage());
        send_test.at(type) = SendTestResult::Failed;
        return;
    }

    auto type_ptr = new SendTestType(type);

    synth_input.setCallback(Callback::receiveTestSysex, type_ptr);
    synth_input.ignoreTypes(false, false, false);

    // set timer for connection timeout
    waiting_timer = SDL_AddTimer(TIMEOUT_DURATION, Callback::timeoutTest, type_ptr);

    addProcessedHistory(true, synth_output.getPortName(), request);
}

bool isAnyTestSending() noexcept
{
    return
        send_test.at(SendTestType::Inquiry) == SendTestResult::WaitReceive ||
        send_test.at(SendTestType::GlobalDump) == SendTestResult::WaitReceive ||
        send_test.at(SendTestType::SoundDump) == SendTestResult::WaitReceive;
}

} // Debug
} // Connector
} // StreichfettSse
#endif