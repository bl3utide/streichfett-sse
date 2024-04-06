#include "common.hpp"
#include "annotation.hpp"
#include "error.hpp"
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
#include "logger.hpp"
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
bool force_adjust_midi_channel = true;
int store_delay_duration = 200;

// private
const int TIMEOUT_DURATION = 5000;
std::string ERROR_MESSAGE_TIMEOUT_CONFIRM = "The confirm sysex request has timed out.";
std::string ERROR_MESSAGE_TIMEOUT_GLOBAL_DUMP = "The global dump request has timed out.";
std::string ERROR_MESSAGE_TIMEOUT_SOUND_DUMP = "The sound dump request has timed out.";
bool _is_synth_connected = false;

void fetchDeviceList()
{
    // MIDI IN Port
    in_name_list.clear();
    for (unsigned int i = 0; i < synth_input.getPortCount(); ++i)
    {
        try
        {
            in_name_list.push_back(synth_input.getPortName(i));
            in_name_list[i] = in_name_list[i].substr(0, in_name_list[i].find_last_of(" "));
        }
        catch (RtMidiError& error)
        {
#ifdef _DEBUG
            LOGD << error.getMessage();
#endif
            throw error;
        }
    }

    // MIDI OUT Port
    out_name_list.clear();
    for (unsigned int i = 0; i < synth_output.getPortCount(); ++i)
    {
        try
        {
            out_name_list.push_back(synth_output.getPortName(i));
            out_name_list[i] = out_name_list[i].substr(0, out_name_list[i].find_last_of(" "));
        }
        catch (RtMidiError& error)
        {
#ifdef _DEBUG
            LOGD << error.getMessage();
#endif
            throw error;
        }
    }
}

void sendDelay(const State next_state, const int delay_millsec)
{
    State* next_state_ptr = new State(next_state);
    _waiting_timer = SDL_AddTimer(delay_millsec, sendDelayCallback, next_state_ptr);

    setNextState(State::WaitingSendDelay);
}

void initialize()
{
    synth_input.initialize();
    synth_output.initialize();
    key_input.initialize();
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
    const std::string cv_synth_input_device_name = Config::getConfigValue<std::string>(Config::Key::SynthInputDevice);
    const auto synth_in_res = std::find(in_name_list.cbegin(), in_name_list.cend(), cv_synth_input_device_name);
    if (synth_in_res != in_name_list.cend())
    {   // found
        const int index = static_cast<int>(std::distance(in_name_list.cbegin(), synth_in_res));
        openSynthInputPort(index, in_name_list[index]);
    }

    // Synth Output Device
    const std::string cv_synth_output_device_name = Config::getConfigValue<std::string>(Config::Key::SynthOutputDevice);
    const auto synth_out_res = std::find(out_name_list.cbegin(), out_name_list.cend(), cv_synth_output_device_name);
    if (synth_out_res != out_name_list.cend())
    {   // found
        const int index = static_cast<int>(std::distance(out_name_list.cbegin(), synth_out_res));
        openSynthOutputPort(index, out_name_list[index]);
    }

    // Keyboard Input Device
    const std::string cv_keyboard_input_device_name = Config::getConfigValue<std::string>(Config::Key::KeyboardInputDevice);
    const auto key_in_res = std::find(in_name_list.cbegin(), in_name_list.cend(), cv_keyboard_input_device_name);
    if (key_in_res != in_name_list.cend())
    {   // found
        const int synth_in_index = static_cast<int>(std::distance(in_name_list.cbegin(), synth_in_res));
        const int key_in_index = static_cast<int>(std::distance(in_name_list.cbegin(), key_in_res));
        if (synth_in_index != key_in_index)
        {
            openKeyInputPort(key_in_index, in_name_list[key_in_index]);
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

void openSynthInputPort(const int port_index, const std::string& port_name)
{
    try
    {
        synth_input.open(port_index, port_name);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(StringUtil::format("MIDI error: %s", error.getMessage().c_str()));
        setSynthConnected(false);
        return;
    }

    if (synth_output.isPortOpen())
    {
        setSynthConnected(true);
        setNextState(State::RequestInquiry);
    }
}

void openSynthOutputPort(const int port_index, const std::string& port_name)
{
    try
    {
        synth_output.open(port_index, port_name);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(StringUtil::format("MIDI error: %s", error.getMessage().c_str()));
        setSynthConnected(false);
        return;
    }

    if (synth_input.isPortOpen())
    {
        setSynthConnected(true);
        setNextState(State::RequestInquiry);
    }
}

void openKeyInputPort(const int port_index, const std::string& port_name)
{
    try
    {
        key_input.open(port_index, port_name);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(StringUtil::format("MIDI error: %s", error.getMessage().c_str()));
        return;
    }

    // receive message in callback function
    key_input.setCallback(receiveKeyDeviceMessageCallback);
    key_input.ignoreTypes(false, false, false);
}

void requestInquiry()
{
    MessageHandler::inquiry_dump.received = false;

    // send sysex message to device via midi out device
    ByteVec confirm_req_sysex =
        MessageHandler::getInquiryRequestMessage();
    synth_output.sendMessage(&confirm_req_sysex);

    // then receive result message in callback function
    synth_input.setCallback(receiveConfirmSysexCallback);
    synth_input.ignoreTypes(false, false, false);

    std::string* err_message_ptr = &ERROR_MESSAGE_TIMEOUT_CONFIRM;

    // set timer for connection timeout
    _waiting_timer = SDL_AddTimer(TIMEOUT_DURATION, timeoutCallback, err_message_ptr);

    setNextState(State::WaitingConfirm);
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_output.getPortName(), confirm_req_sysex);
#endif
}

void requestGlobalData()
{
    ByteVec global_req_sysex =
        MessageHandler::getGlobalRequestMessage();

    try
    {
        synth_output.sendMessage(&global_req_sysex);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(error.getMessage().c_str());
        setNextState(State::Idle);
        setSynthConnected(false);
        return;
    }

    synth_input.setCallback(receiveGlobalDumpSysexCallback);
    synth_input.ignoreTypes(false, false, false);

    std::string* err_message_ptr = &ERROR_MESSAGE_TIMEOUT_GLOBAL_DUMP;

    // set timer for connection timeout
    _waiting_timer = SDL_AddTimer(TIMEOUT_DURATION, timeoutCallback, err_message_ptr);

    setNextState(State::WaitingGlobal);
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_output.getPortName(), global_req_sysex);
#endif
}

// DSI: Streichfett
void requestSoundData()
{
    InternalPatch::SoundAddress* sound_addr = InternalPatch::getCurrentSoundAddress();

    ByteVec sound_req_sysex =
        MessageHandler::getSoundRequestMessage(sound_addr->sound);

    try
    {
        synth_output.sendMessage(&sound_req_sysex);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(error.getMessage().c_str());
        setNextState(State::Idle);
        setSynthConnected(false);
        return;
    }

    synth_input.setCallback(receiveSoundDumpSysexCallback);
    synth_input.ignoreTypes(false, false, false);

    std::string* err_message_ptr = &ERROR_MESSAGE_TIMEOUT_SOUND_DUMP;

    // set timer for connection timeout
    _waiting_timer = SDL_AddTimer(TIMEOUT_DURATION, timeoutCallback, err_message_ptr);

    setNextState(State::WaitingSound);
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_output.getPortName(), sound_req_sysex);
#endif
}

// DSI: Streichfett
void sendSoundDump(const bool is_edit_buffer)
{
    InternalPatch::SoundAddress* sound_addr =
        InternalPatch::getCurrentSoundAddress();
    int sound = is_edit_buffer ? -1 : sound_addr->sound;
    SoundModel::Patch* current_patch = InternalPatch::getCurrentPatch();

    ByteVec sound_dump =
        MessageHandler::getSoundDumpMessageFromPatch(sound, current_patch);

    try
    {
        synth_output.sendMessage(&sound_dump);
        _is_waiting_store_delay = true;
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(error.getMessage().c_str());
        setNextState(State::Idle);
        setSynthConnected(false);
        return;
    }

    InternalPatch::SoundAddress* sound_address_ptr = new InternalPatch::SoundAddress(sound);

    // set timer for delay after sending sound dump
    _waiting_timer = SDL_AddTimer(store_delay_duration, storeDelayCallback, sound_address_ptr);

#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_output.getPortName(), sound_dump);
#endif
}

// DSI: Streichfett
void sendProgChange()
{
    InternalPatch::SoundAddress* sound_addr = InternalPatch::getCurrentSoundAddress();

    ByteVec prog_change =
        MessageHandler::getProgChangeMessage(sound_addr->sound);

    try
    {
        synth_output.sendMessage(&prog_change);
        sendDelay(State::RequestSound, 150);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(error.getMessage().c_str());
        setNextState(State::Idle);
        setSynthConnected(false);
        return;
    }
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_output.getPortName(), prog_change);
#endif
}

void sendAllSoundOff()
{
    ByteVec all_sound_off = MessageHandler::getAllSoundOffMessage();

    try
    {
        synth_output.sendMessage(&all_sound_off);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(error.getMessage().c_str());
        return;
    }
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_output.getPortName(), all_sound_off);
#endif
}

void sendOneTaskMessage()
{
    if (MessageTask::taskSize() > 0)
    {
        ByteVec message = MessageTask::lastTask();
        synth_output.sendMessage(&message);
#ifdef _DEBUG
        Debug::addProcessedHistory(true, synth_output.getPortName(), message);
#endif
    }
}

bool isSynthConnected() noexcept
{
    return _is_synth_connected;
}

void setSynthConnected(const bool connected) noexcept
{
    if (connected)
    {
        _is_synth_connected = true;
        Annotation::clearText();
    }
    else
    {
        _is_synth_connected = false;
        Annotation::setText(Annotation::Message::DisconnectedAlert, Annotation::Type::Alert);
    }
}

} // Connector
} // StreichfettSse
