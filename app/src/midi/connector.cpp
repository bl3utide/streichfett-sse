#include "common.hpp"
#include "annotation.hpp"
#include "error.hpp"
#include "operation.hpp"
#include "state.hpp"
#include "data/internal_patch.hpp"
#include "data/internal_setting.hpp"
#include "midi/callback.hpp"
#include "midi/connector.hpp"
#include "midi/connector_common.hpp"
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
MidiConnection synth_conn;
MidiConnection key_conn;
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
    unsigned int inPortNum = synth_conn.input->getPortCount();
    for (unsigned int i = 0; i < inPortNum; ++i)
    {
        try
        {
            in_name_list.push_back(synth_conn.input->getPortName(i));
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
    unsigned int outPortNum = synth_conn.output->getPortCount();
    for (unsigned int i = 0; i < outPortNum; ++i)
    {
        try
        {
            out_name_list.push_back(synth_conn.output->getPortName(i));
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
    synth_conn.initialize();
    key_conn.initialize();
}

void finalize() noexcept
{
    synth_conn.finalize();
    key_conn.finalize();
}

void resetAllConnections()
{
    synth_conn.closePorts();
    synth_conn.resetPortInfo();
    key_conn.closePorts();
    key_conn.resetPortInfo();
    fetchDeviceList();
    setSynthConnected(false);
}

void checkOpenPorts()
{
    if (synth_conn.input_port_index == -1 || synth_conn.output_port_index == -1)
        return;

    try
    {
        synth_conn.openPorts();
        setNextState(State::RequestInquiry);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(format("MIDI error: %s", error.getMessage().c_str()));
        setNextState(State::Idle);
        setSynthConnected(false);
        return;
    }
}

void keyOpenPort()
{
    key_conn.input->cancelCallback();

    try
    {
        key_conn.openInPort();
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(format("MIDI error: %s", error.getMessage().c_str()));
        return;
    }

    // receive message in callback function
    key_conn.input->setCallback(receiveKeyDeviceMessageCallback);
    key_conn.input->ignoreTypes(false, false, false);
}

void requestInquiry()
{
    MessageHandler::inquiry_dump.received = false;

    // send sysex message to device via midi out device
    Bytes confirm_req_sysex =
        MessageHandler::getInquiryRequestMessage();
    synth_conn.output->sendMessage(&confirm_req_sysex);

    // then receive result message in callback function
    synth_conn.input->setCallback(receiveConfirmSysexCallback);
    synth_conn.input->ignoreTypes(false, false, false);

    std::string* err_message_ptr = &ERROR_MESSAGE_TIMEOUT_CONFIRM;

    // set timer for connection timeout
    _waiting_timer = SDL_AddTimer(TIMEOUT_DURATION, timeoutCallback, err_message_ptr);

    setNextState(State::WaitingConfirm);
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_conn.output_port_name, confirm_req_sysex);
#endif
}

void requestGlobalData()
{
    Bytes global_req_sysex =
        MessageHandler::getGlobalRequestMessage();

    try
    {
        synth_conn.output->sendMessage(&global_req_sysex);
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

    synth_conn.input->setCallback(receiveGlobalDumpSysexCallback);
    synth_conn.input->ignoreTypes(false, false, false);

    std::string* err_message_ptr = &ERROR_MESSAGE_TIMEOUT_GLOBAL_DUMP;

    // set timer for connection timeout
    _waiting_timer = SDL_AddTimer(TIMEOUT_DURATION, timeoutCallback, err_message_ptr);

    setNextState(State::WaitingGlobal);
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_conn.output_port_name, global_req_sysex);
#endif
}

// DSI: Streichfett
void requestSoundData()
{
    InternalPatch::SoundAddress* sound_addr = InternalPatch::getCurrentSoundAddress();

    Bytes sound_req_sysex =
        MessageHandler::getSoundRequestMessage(sound_addr->sound);

    try
    {
        synth_conn.output->sendMessage(&sound_req_sysex);
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

    synth_conn.input->setCallback(receiveSoundDumpSysexCallback);
    synth_conn.input->ignoreTypes(false, false, false);

    std::string* err_message_ptr = &ERROR_MESSAGE_TIMEOUT_SOUND_DUMP;

    // set timer for connection timeout
    _waiting_timer = SDL_AddTimer(TIMEOUT_DURATION, timeoutCallback, err_message_ptr);

    setNextState(State::WaitingSound);
#ifdef _DEBUG
    Debug::addProcessedHistory(true, synth_conn.output_port_name, sound_req_sysex);
#endif
}

// DSI: Streichfett
void sendSoundDump(const bool is_edit_buffer)
{
    InternalPatch::SoundAddress* sound_addr =
        InternalPatch::getCurrentSoundAddress();
    int sound = is_edit_buffer ? -1 : sound_addr->sound;
    SoundModel::Patch* current_patch = InternalPatch::getCurrentPatch();

    Bytes sound_dump =
        MessageHandler::getSoundDumpMessageFromPatch(sound, current_patch);

    try
    {
        synth_conn.output->sendMessage(&sound_dump);
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
    Debug::addProcessedHistory(true, synth_conn.output_port_name, sound_dump);
#endif
}

// DSI: Streichfett
void sendProgChange()
{
    InternalPatch::SoundAddress* sound_addr = InternalPatch::getCurrentSoundAddress();

    Bytes prog_change =
        MessageHandler::getProgChangeMessage(sound_addr->sound);

    try
    {
        synth_conn.output->sendMessage(&prog_change);
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
    Debug::addProcessedHistory(true, synth_conn.output_port_name, prog_change);
#endif
}

void sendAllSoundOff()
{
    Bytes all_sound_off = MessageHandler::getAllSoundOffMessage();

    try
    {
        synth_conn.output->sendMessage(&all_sound_off);
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
    Debug::addProcessedHistory(true, synth_conn.output_port_name, all_sound_off);
#endif
}

void sendOneTaskMessage()
{
    if (MessageTask::taskSize() > 0)
    {
        Bytes message = MessageTask::lastTask();
        synth_conn.output->sendMessage(&message);
#ifdef _DEBUG
        Debug::addProcessedHistory(true, synth_conn.output_port_name, message);
#endif
    }
}

bool isWaitingStoreDelay() noexcept
{
    return _is_waiting_store_delay;
}

bool isSynthConnected() noexcept
{
    return _is_synth_connected;
}

void setSynthConnected(const bool connected) noexcept
{
    if (connected)
        _is_synth_connected = true;
    else
    {
        _is_synth_connected = false;
        Annotation::setText(Annotation::Message::DisconnectedAlert, Annotation::Type::Alert);
    }
}

} // Connector
} // StreichfettSse
