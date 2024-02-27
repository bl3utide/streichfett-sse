#include "common.hpp"
#include "annotation.hpp"
#include "error.hpp"
#include "operation.hpp"
#include "state.hpp"
#include "data/internal_patch.hpp"
#include "data/internal_setting.hpp"
#include "midi/connector.hpp"
#include "midi/message_task.hpp"
#ifdef _DEBUG
#include "logger.hpp"
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
#ifdef _DEBUG
SendTestResult send_test[static_cast<int>(SendTestType::_COUNT_)];
SendTestFailedCause send_test_failed_cause[static_cast<int>(SendTestType::_COUNT_)];
std::list<ProcessedMidiMessage> processed_history;
int history_selected_index = -1;
ProcessedMidiMessage selected_processed_message;
#endif

// private
bool _is_waiting_store_delay = false;   // store delay check
SDL_TimerID _waiting_timer;
const int TIMEOUT_DURATION = 5000;
std::string ERROR_MESSAGE_TIMEOUT_CONFIRM = "The confirm sysex request has timed out.";
std::string ERROR_MESSAGE_TIMEOUT_GLOBAL_DUMP = "The global dump request has timed out.";
std::string ERROR_MESSAGE_TIMEOUT_SOUND_DUMP = "The sound dump request has timed out.";
bool _is_synth_connected = false;
#ifdef _DEBUG
size_t _processed_history_max_size = 100;
#endif

#ifdef _DEBUG
void addProcessedHistory(const bool transmitted, const std::string& device_name, const MessageHandler::Bytes& data)
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
#endif

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

/*******************************************************************************
    Received confirm response callback
*******************************************************************************/
void receiveConfirmSysexCallback(double delta_time, MessageHandler::Bytes* message, void* user_data)
{
    if (message->empty())
    {
#ifdef _DEBUG
        LOGD << "Received dump was empty message";
#endif
    }
    else
    {
        if (MessageHandler::checkInquiryDump(*message))
        {
            Annotation::clearText();
            setNextState(State::RequestGlobal);
            setSynthConnected(true);
        }
        else
        {
            setAppError("You tried to connect to an incorrect device.");
            setNextState(State::Idle);
            setSynthConnected(false);
        }
    }

    synth_conn.input->cancelCallback();
    SDL_RemoveTimer(_waiting_timer);

#ifdef _DEBUG
    addProcessedHistory(false, synth_conn.input_port_name, *message);
#endif
}

/*******************************************************************************
    Received global dump response callback
*******************************************************************************/
// DSI: Streichfett
void receiveGlobalDumpSysexCallback(double delta_time, MessageHandler::Bytes* message, void* user_data)
{
    if (message->empty())
    {
#ifdef _DEBUG
        LOGD << "Received dump was empty message";
#endif
    }
    else
    {
        MessageHandler::DumpType dump_type = MessageHandler::DumpType::Global;

        try
        {
            // throwable function
            MessageHandler::checkDump(*message, dump_type);

            MessageHandler::Bytes global_data =
                MessageHandler::getDataBytesFromDump(*message, dump_type);
            GlobalModel::Global* setting = InternalSetting::getGlobalData();

            // throwable function
            InternalSetting::setSettingFromBytes(setting, global_data);

            Operation op = getOperation();
            if (op == Operation::Sound)
                setNextState(State::SendBankProgChange);
            else if (op == Operation::Option)
                setNextState(State::Idle);
        }
        catch (std::exception&)
        {
            setAppError("Incorrect Global Dump Message");
            setNextState(State::Idle);
            setSynthConnected(false);
        }
    }

    synth_conn.input->cancelCallback();
    SDL_RemoveTimer(_waiting_timer);

#ifdef _DEBUG
    addProcessedHistory(false, synth_conn.input_port_name, *message);
#endif
}

/*******************************************************************************
    Received sound dump response callback
*******************************************************************************/
// DSI: Streichfett
void receiveSoundDumpSysexCallback(double delta_time, MessageHandler::Bytes* message, void* user_data)
{
    if (message->empty())
    {
#ifdef _DEBUG
        LOGD << "Received dump was empty message";
#endif
    }
    else
    {
        MessageHandler::DumpType dump_type = MessageHandler::DumpType::Sound;

        try
        {
            // throwable function
            MessageHandler::checkDump(*message, dump_type);

            MessageHandler::Bytes sound_data =
                MessageHandler::getDataBytesFromDump(*message, dump_type);
            SoundModel::Patch* original = InternalPatch::getOriginalPatch();
            SoundModel::Patch* current = InternalPatch::getCurrentPatch();

            // throwable function
            InternalPatch::setPatchFromBytes(original, sound_data);
            InternalPatch::copyPatchAtoB(original, current);

            InternalPatch::SoundAddress* sound_addr = InternalPatch::getCurrentSoundAddress();
            char bb = InternalPatch::getSoundBankChar(sound_addr->sound);
            int bs = InternalPatch::getSoundPatchNumber(sound_addr->sound);
            char buf[64];
            sprintf(buf, "Sound %c%d loaded.", bb, bs);
            Annotation::setText(buf, Annotation::Type::General);

            setNextState(State::Idle);
        }
        catch (std::exception&)
        {
            setAppError("Incorrect Sound Dump Message");
        }
    }

    synth_conn.input->cancelCallback();
    SDL_RemoveTimer(_waiting_timer);

#ifdef _DEBUG
    addProcessedHistory(false, synth_conn.input_port_name, *message);
#endif
}

/*******************************************************************************
    Timeout response callback (General purpose)
*******************************************************************************/
Uint32 timeoutCallback(Uint32 interval, void* param)
{
    std::string err_message = *static_cast<std::string*>(param);

    synth_conn.input->cancelCallback();
    SDL_RemoveTimer(_waiting_timer);
    setAppError(err_message);
    setNextState(State::Idle);
    setSynthConnected(false);

    return interval;
}

/*******************************************************************************
    Received message from key-device callback
*******************************************************************************/
// DSI: Streichfett
void receiveKeyDeviceMessageCallback(double delta_time, MessageHandler::Bytes* message, void* user_data)
{
    if (isSynthConnected() &&
        (MessageHandler::isNoteOff(*message) || MessageHandler::isNoteOn(*message)))
    {
        if (Connector::force_adjust_midi_channel)
        {
            const int ch = InternalSetting::getDeviceMidiChannel();
            MessageHandler::Bytes channel_adj_message;
            if (MessageHandler::isNoteOff(*message))
            {
                channel_adj_message = {
                    static_cast<unsigned char>(0x80 + ch),
                    message->at(1),
                    message->at(2)
                };
            }
            else
            {
                channel_adj_message = {
                    static_cast<unsigned char>(0x90 + ch),
                    message->at(1),
                    message->at(2)
                };
            }
            synth_conn.output->sendMessage(&channel_adj_message);
        }
        else
        {
            synth_conn.output->sendMessage(message);
        }
    }
}

/*******************************************************************************
    Store delay callback
*******************************************************************************/
// DSI: Streichfett
Uint32 storeDelayCallback(Uint32 interval, void* param)
{
    InternalPatch::SoundAddress* sound_address_ptr
        = static_cast<InternalPatch::SoundAddress*>(param);

    int sound = sound_address_ptr->sound;

    // -1: sent to edit buffer
    if (sound != -1)
    {
        SoundModel::Patch* original = InternalPatch::getOriginalPatch();
        SoundModel::Patch* current = InternalPatch::getCurrentPatch();
        InternalPatch::copyPatchAtoB(current, original);

        char bb = InternalPatch::getSoundBankChar(sound);
        int bs = InternalPatch::getSoundPatchNumber(sound);
        char buf[64];
        sprintf(buf, "The sound has stored to %c%d", bb, bs);
        Annotation::setText(buf, Annotation::Type::General);
        InternalPatch::current_patch_changed = false;
    }

    SDL_RemoveTimer(_waiting_timer);
    _is_waiting_store_delay = false;

    delete sound_address_ptr;
    sound_address_ptr = nullptr;

    return interval;
}

Uint32 sendDelayCallback(Uint32 interval, void* param)
{
    State* next_state_ptr = static_cast<State*>(param);

    setNextState(*next_state_ptr);

    SDL_RemoveTimer(_waiting_timer);

    delete next_state_ptr;
    next_state_ptr = nullptr;

    return interval;
}

void sendDelay(const State next_state, const int delay_millsec)
{
    State* next_state_ptr = new State(next_state);
    _waiting_timer = SDL_AddTimer(delay_millsec, sendDelayCallback, next_state_ptr);

    setNextState(State::WaitingSendDelay);
}

#ifdef _DEBUG
void receiveTestSysexCallback(double delta_time, MessageHandler::Bytes* message, void* user_data)
{
    SendTestType* type_ptr = static_cast<SendTestType*>(user_data);
    int type_index = static_cast<int>(*type_ptr);

    if (message->empty())
    {
        send_test[type_index] = SendTestResult::Failed;
        send_test_failed_cause[type_index] = SendTestFailedCause::EmptyResponse;
    }
    else
    {
        switch (*type_ptr)
        {
            case SendTestType::Inquiry:
                if (MessageHandler::checkInquiryDump(*message))
                    send_test[type_index] = SendTestResult::Ok;
                else
                {
                    send_test[type_index] = SendTestResult::Failed;
                    send_test_failed_cause[type_index] = SendTestFailedCause::IncorrectMessage;
                }
                break;
            case SendTestType::GlobalDump:
                try
                {
                    MessageHandler::checkDump(*message, MessageHandler::DumpType::Global);
                    send_test[type_index] = SendTestResult::Ok;
                }
                catch (std::exception&)
                {
                    send_test[type_index] = SendTestResult::Failed;
                    send_test_failed_cause[type_index] = SendTestFailedCause::IncorrectMessage;
                }
                break;
            case SendTestType::SoundDump:
                try
                {
                    MessageHandler::checkDump(*message, MessageHandler::DumpType::Sound);
                    send_test[type_index] = SendTestResult::Ok;
                }
                catch (std::exception&)
                {
                    send_test[type_index] = SendTestResult::Failed;
                    send_test_failed_cause[type_index] = SendTestFailedCause::IncorrectMessage;
                }
                break;
        }
    }

    synth_conn.input->cancelCallback();
    SDL_RemoveTimer(_waiting_timer);

    delete type_ptr;
    type_ptr = nullptr;

    addProcessedHistory(false, synth_conn.input_port_name, *message);
}
#endif

#ifdef _DEBUG
Uint32 timeoutTestCallback(Uint32 interval, void* param)
{
    SendTestType* type_ptr = static_cast<SendTestType*>(param);
    int type_index = static_cast<int>(*type_ptr);

    synth_conn.input->cancelCallback();
    SDL_RemoveTimer(_waiting_timer);
    send_test[type_index] = SendTestResult::Failed;
    send_test_failed_cause[type_index] = SendTestFailedCause::RequestTimeout;

    delete type_ptr;
    type_ptr = nullptr;

    return interval;
}
#endif

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

#ifdef _DEBUG
// DSI: Streichfett
void sendTest(const SendTestType type)
{
    for (int i = 0; i < static_cast<int>(SendTestType::_COUNT_); ++i)
        send_test[i] = SendTestResult::NotStarted;

    MessageHandler::Bytes request;

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
        synth_conn.output->sendMessage(&request);
    }
    catch (RtMidiError& error)
    {
        LOGD << error.getMessage();
        send_test[static_cast<int>(type)] = SendTestResult::Failed;
        return;
    }

    SendTestType* type_ptr = new SendTestType(type);

    synth_conn.input->setCallback(receiveTestSysexCallback, type_ptr);
    synth_conn.input->ignoreTypes(false, false, false);

    // set timer for connection timeout
    _waiting_timer = SDL_AddTimer(TIMEOUT_DURATION, timeoutTestCallback, type_ptr);

    addProcessedHistory(true, synth_conn.output_port_name, request);
}
#endif

#ifdef _DEBUG
bool isAnyTestSending() noexcept
{
    return
        Connector::send_test[static_cast<int>(Connector::SendTestType::Inquiry)] == Connector::SendTestResult::WaitReceive ||
        Connector::send_test[static_cast<int>(Connector::SendTestType::GlobalDump)] == Connector::SendTestResult::WaitReceive ||
        Connector::send_test[static_cast<int>(Connector::SendTestType::SoundDump)] == Connector::SendTestResult::WaitReceive;
}
#endif

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
    MessageHandler::Bytes confirm_req_sysex =
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
    addProcessedHistory(true, synth_conn.output_port_name, confirm_req_sysex);
#endif
}

void requestGlobalData()
{
    MessageHandler::Bytes global_req_sysex =
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
    addProcessedHistory(true, synth_conn.output_port_name, global_req_sysex);
#endif
}

// DSI: Streichfett
void requestSoundData()
{
    InternalPatch::SoundAddress* sound_addr = InternalPatch::getCurrentSoundAddress();

    MessageHandler::Bytes sound_req_sysex =
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
    addProcessedHistory(true, synth_conn.output_port_name, sound_req_sysex);
#endif
}

// DSI: Streichfett
void sendSoundDump(const bool is_edit_buffer)
{
    InternalPatch::SoundAddress* sound_addr =
        InternalPatch::getCurrentSoundAddress();
    int sound = is_edit_buffer ? -1 : sound_addr->sound;
    SoundModel::Patch* current_patch = InternalPatch::getCurrentPatch();

    MessageHandler::Bytes sound_dump =
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
    addProcessedHistory(true, synth_conn.output_port_name, sound_dump);
#endif
}

// DSI: Streichfett
void sendProgChange()
{
    InternalPatch::SoundAddress* sound_addr = InternalPatch::getCurrentSoundAddress();

    MessageHandler::Bytes prog_change =
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
    addProcessedHistory(true, synth_conn.output_port_name, prog_change);
#endif
}

void sendAllSoundOff()
{
    MessageHandler::Bytes all_sound_off = MessageHandler::getAllSoundOffMessage();

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
    addProcessedHistory(true, synth_conn.output_port_name, all_sound_off);
#endif
}

void sendOneTaskMessage()
{
    if (MessageTask::taskSize() > 0)
    {
        MessageHandler::Bytes message = MessageTask::lastTask();
        synth_conn.output->sendMessage(&message);
#ifdef _DEBUG
        addProcessedHistory(true, synth_conn.output_port_name, message);
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
