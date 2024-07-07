#include "common.hpp"
#include "annotation.hpp"
#include "error.hpp"
#include "logger.hpp"
#include "operation.hpp"
#include "state.hpp"
#include "config/config.hpp"
#include "data/internal_patch.hpp"
#include "data/internal_setting.hpp"
#include "midi/midi_common.hpp"
#include "midi/callback.hpp"
#include "midi/connector.hpp"
#include "midi/message_handler.hpp"
#include "midi/message_task.hpp"
#ifdef _DEBUG
#include "midi/connector_debug.hpp"
#endif

namespace StreichfettSse
{
namespace Connector
{

// public
InputConnection synth_input;
OutputConnection synth_output;
InputConnection key_input;
std::vector<std::string> in_name_list;
std::vector<std::string> out_name_list;
bool force_adjust_midi_channel;
int store_delay_duration;

// private
bool is_synth_connected_;

void fetchDeviceList()
{
    // MIDI IN Port
    in_name_list.clear();
    const auto in_port_count = synth_input.getPortCount();
    for (std::uint32_t i = 0; i < in_port_count; ++i)
    {
        try
        {
            in_name_list.push_back(synth_input.getPortName(i));
            in_name_list[i] = in_name_list[i].substr(0, in_name_list[i].find_last_of(" "));
        }
        catch (RtMidiError& error)
        {
            throw ContinuableException(
                StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
                ERROR_WHEN_FETCH_DEV_LIST,
                ERROR_CAUSE_GET_INDEV_NAME
            );
        }
    }

    // MIDI OUT Port
    out_name_list.clear();
    const auto out_port_count = synth_output.getPortCount();
    for (std::uint32_t i = 0; i < out_port_count; ++i)
    {
        try
        {
            out_name_list.push_back(synth_output.getPortName(i));
            out_name_list[i] = out_name_list[i].substr(0, out_name_list[i].find_last_of(" "));
        }
        catch (RtMidiError& error)
        {
            throw ContinuableException(
                StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
                ERROR_WHEN_FETCH_DEV_LIST,
                ERROR_CAUSE_GET_OUTDEV_NAME
            );
        }
    }
}

void sendDelay(State next_state, int delay_millsec)
{
    auto next_state_ptr = new State(next_state);
    waiting_timer = SDL_AddTimer(delay_millsec, Callback::sendDelay, next_state_ptr);

    setNextState(State::WaitingSendDelay);
}

void initialize()
{
    synth_input.initialize();
    synth_output.initialize();
    key_input.initialize();

    request_try_count.reset();

    force_adjust_midi_channel = true;
    store_delay_duration = 200;
    is_synth_connected_ = false;

    setWaitingStoreDelay(false);

#ifdef _DEBUG
    for (auto i = 0; i < static_cast<int>(Debug::SendTestType::_COUNT_); ++i)
    {
        Debug::send_test.emplace(static_cast<Debug::SendTestType>(i), Debug::SendTestResult::NotStarted);
        Debug::send_test_failed_cause.emplace(static_cast<Debug::SendTestType>(i), Debug::SendTestFailedCause::None);
    }

    Debug::history_selected_index = -1;
#endif
}

void finalize() noexcept
{
    synth_input.finalize();
    synth_output.finalize();
    key_input.finalize();
}

void applyConfig()
{
    // Synth Input Device
    const auto cv_synth_input_device_name = Config::getConfigValue<std::string>(Config::Key::SynthInputDevice);
    const auto synth_in_res = std::find(in_name_list.cbegin(), in_name_list.cend(), cv_synth_input_device_name);
    if (synth_in_res != in_name_list.cend())
    {   // found
        const int index = static_cast<int>(std::distance(in_name_list.cbegin(), synth_in_res));
        try
        {
            openSynthInputPort(index, in_name_list[index]);
        }
        catch (ContinuableException& ce)
        {
            Logger::debug(ce);
        }
    }

    // Synth Output Device
    const auto cv_synth_output_device_name = Config::getConfigValue<std::string>(Config::Key::SynthOutputDevice);
    const auto synth_out_res = std::find(out_name_list.cbegin(), out_name_list.cend(), cv_synth_output_device_name);
    if (synth_out_res != out_name_list.cend())
    {   // found
        const auto index = static_cast<int>(std::distance(out_name_list.cbegin(), synth_out_res));
        try
        {
            openSynthOutputPort(index, out_name_list[index]);
        }
        catch (ContinuableException& ce)
        {
            Logger::debug(ce);
        }
    }

    // Keyboard Input Device
    const auto cv_keyboard_input_device_name = Config::getConfigValue<std::string>(Config::Key::KeyboardInputDevice);
    const auto key_in_res = std::find(in_name_list.cbegin(), in_name_list.cend(), cv_keyboard_input_device_name);
    if (key_in_res != in_name_list.cend())
    {   // found
        const auto synth_in_index = static_cast<int>(std::distance(in_name_list.cbegin(), synth_in_res));
        const auto key_in_index = static_cast<int>(std::distance(in_name_list.cbegin(), key_in_res));
        if (synth_in_index != key_in_index)
        {
            try
            {
                openKeyInputPort(key_in_index, in_name_list[key_in_index]);
            }
            catch (ContinuableException& ce)
            {
                Logger::debug(ce);
            }
        }
    }

    // Force Adjust MIDI Channel
    force_adjust_midi_channel = Config::getConfigValue<bool>(Config::Key::ForceAdjustMidiCh);

    // SysEx Delay
    store_delay_duration = Config::getConfigValue<int>(Config::Key::SysExDelay);
}

void updateConfig() noexcept
{
    Config::setConfigValue(Config::Key::SynthInputDevice, synth_input.getPortName());
    Config::setConfigValue(Config::Key::SynthOutputDevice, synth_output.getPortName());
    Config::setConfigValue(Config::Key::KeyboardInputDevice, key_input.getPortName());
    Config::setConfigValue(Config::Key::ForceAdjustMidiCh, force_adjust_midi_channel);
    Config::setConfigValue(Config::Key::SysExDelay, store_delay_duration);
}

void resetAllConnections()
{
    synth_input.close();
    synth_output.close();
    key_input.close();
    fetchDeviceList();
    setSynthConnected(false);
}

void openSynthInputPort(int port_index, const std::string& port_name)
{
    try
    {
        synth_input.open(port_index, port_name);
    }
    catch (RtMidiError& error)
    {
        setSynthConnected(false);
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_OPEN_DEV, ERROR_CAUSE_OPEN_DEV_SI);
    }

    if (synth_output.isPortOpen())
    {
        setSynthConnected(true);
        setNextState(State::RequestInquiry);
    }
}

void openSynthOutputPort(int port_index, const std::string& port_name)
{
    try
    {
        synth_output.open(port_index, port_name);
    }
    catch (RtMidiError& error)
    {
        setSynthConnected(false);
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_OPEN_DEV, ERROR_CAUSE_OPEN_DEV_SO);
    }

    if (synth_input.isPortOpen())
    {
        setSynthConnected(true);
        setNextState(State::RequestInquiry);
    }
}

void openKeyInputPort(int port_index, const std::string& port_name)
{
    try
    {
        key_input.open(port_index, port_name);
    }
    catch (RtMidiError& error)
    {
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_OPEN_DEV, ERROR_CAUSE_OPEN_DEV_KI);
    }

    // receive message in callback function
    key_input.setCallback(Callback::receiveKeyDeviceMessage);
    key_input.ignoreTypes(false, false, false);
}

void requestInquiry()
{
    MessageHandler::inquiry_dump.received = false;

    // send sysex message to device via midi out device
    const auto confirm_req_sysex = MessageHandler::getInquiryRequestMessage();

    Logger::debug(StringUtil::format("request inquiry dump [try count: %d/%d]", request_try_count.v() + 1, request_try_count.max()));

    try
    {
        synth_output.sendMessage(confirm_req_sysex);
    }
    catch (RtMidiError& error)
    {
        setSynthConnected(false);
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_SEND_MESSAGE,
            ERROR_CAUSE_REQ_INQUIRY,
            State::Idle
        );
    }

    ++request_try_count;
    callback_mutex.is_callback_catched = false;
    RequestType* req_type_ptr = new RequestType(RequestType::Confirm);

    // then receive result message in callback function
    synth_input.setCallback(Callback::receiveConfirmSysex, req_type_ptr);
    synth_input.ignoreTypes(false, false, false);

    // set timer for connection timeout
    waiting_timer = SDL_AddTimer(TIMEOUT_DURATION_PER_RETRY, Callback::timeout, req_type_ptr);

    setNextState(State::WaitingConfirm);
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_output.getPortName(), confirm_req_sysex);
#endif
}

void requestGlobalData()
{
    const auto global_req_sysex = MessageHandler::getGlobalRequestMessage();

    Logger::debug(StringUtil::format("request global dump [try count: %d/%d]", request_try_count.v() + 1, request_try_count.max()));

    try
    {
        synth_output.sendMessage(global_req_sysex);
    }
    catch (RtMidiError& error)
    {
        setSynthConnected(false);
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_SEND_MESSAGE,
            ERROR_CAUSE_REQ_GLOBAL,
            State::Idle
        );
    }

    ++request_try_count;
    callback_mutex.is_callback_catched = false;
    RequestType* req_type_ptr = new RequestType(RequestType::GlobalDump);

    synth_input.setCallback(Callback::receiveGlobalDumpSysex, req_type_ptr);
    synth_input.ignoreTypes(false, false, false);

    // set timer for connection timeout
    waiting_timer = SDL_AddTimer(TIMEOUT_DURATION_PER_RETRY, Callback::timeout, req_type_ptr);

    setNextState(State::WaitingGlobal);
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_output.getPortName(), global_req_sysex);
#endif
}

// DSI: Streichfett
void requestSoundData()
{
    auto& sound_addr = InternalPatch::getCurrentSoundAddress();

    const auto sound_req_sysex = MessageHandler::getSoundRequestMessage(sound_addr.sound);

    Logger::debug(StringUtil::format("request sound dump [try count: %d/%d]", request_try_count.v() + 1, request_try_count.max()));

    try
    {
        synth_output.sendMessage(sound_req_sysex);
    }
    catch (RtMidiError& error)
    {
        setSynthConnected(false);
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_SEND_MESSAGE,
            ERROR_CAUSE_REQ_SOUND,
            State::Idle
        );
    }

    ++request_try_count;
    callback_mutex.is_callback_catched = false;
    RequestType* req_type_ptr = new RequestType(RequestType::SoundDump);

    synth_input.setCallback(Callback::receiveSoundDumpSysex, req_type_ptr);
    synth_input.ignoreTypes(false, false, false);

    // set timer for connection timeout
    waiting_timer = SDL_AddTimer(TIMEOUT_DURATION_PER_RETRY, Callback::timeout, req_type_ptr);

    setNextState(State::WaitingSound);
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_output.getPortName(), sound_req_sysex);
#endif
}

// DSI: Streichfett
void sendSoundDump(const bool is_edit_buffer)
{
    auto& sound_addr = InternalPatch::getCurrentSoundAddress();
    const auto sound = is_edit_buffer ? -1 : sound_addr.sound;
    const auto& current_patch = InternalPatch::getCurrentPatch();

    const auto sound_dump = MessageHandler::getSoundDumpMessageFromPatch(sound, current_patch);

    try
    {
        synth_output.sendMessage(sound_dump);
        setWaitingStoreDelay(true);
    }
    catch (RtMidiError& error)
    {
        setSynthConnected(false);
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_SEND_MESSAGE,
            ERROR_CAUSE_SOUND_DUMP,
            State::Idle
        );
    }

    InternalPatch::SoundAddress* sound_address_ptr = new InternalPatch::SoundAddress(sound);

    // set timer for delay after sending sound dump
    waiting_timer = SDL_AddTimer(store_delay_duration, Callback::storeDelay, sound_address_ptr);

#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_output.getPortName(), sound_dump);
#endif
}

// DSI: Streichfett
void sendProgChange()
{
    auto& sound_addr = InternalPatch::getCurrentSoundAddress();

    const auto prog_change = MessageHandler::getProgChangeMessage(sound_addr.sound);

    try
    {
        synth_output.sendMessage(prog_change);
        sendDelay(State::RequestSound, 150);
    }
    catch (RtMidiError& error)
    {
        setSynthConnected(false);
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_SEND_MESSAGE,
            ERROR_CAUSE_PROG_CHANGE,
            State::Idle
        );
    }
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_output.getPortName(), prog_change);
#endif
}

void sendAllSoundOff()
{
    const auto all_sound_off = MessageHandler::getAllSoundOffMessage();

    try
    {
        synth_output.sendMessage(all_sound_off);
    }
    catch (RtMidiError& error)
    {
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_SEND_MESSAGE,
            ERROR_CAUSE_ALL_SND_OFF
        );
    }
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_output.getPortName(), all_sound_off);
#endif
}

void sendOneTaskMessage()
{
    if (MessageTask::taskSize() > 0)
    {
        const auto message = MessageTask::lastTask();

        try
        {
            synth_output.sendMessage(message);
        }
        catch (RtMidiError& error)
        {
            throw ContinuableException(
                StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
                ERROR_WHEN_SEND_MESSAGE,
                ERROR_CAUSE_ONE_TASK
            );
        }

#ifdef _DEBUG
        Debug::addProcessedHistory(true, synth_output.getPortName(), message);
#endif
    }
}

bool isSynthConnected() noexcept
{
    return is_synth_connected_;
}

void setSynthConnected(const bool connected) noexcept
{
    if (connected)
    {
        is_synth_connected_ = true;
        Annotation::clearText();
    }
    else
    {
        is_synth_connected_ = false;
        Annotation::setText(Annotation::Message::DisconnectedAlert, Annotation::Type::Alert);
    }
}

} // Connector
} // StreichfettSse
