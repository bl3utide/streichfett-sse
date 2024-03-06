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
#ifdef _DEBUG
#include "midi/connector_debug.hpp"
#endif

namespace StreichfettSse
{
namespace Connector
{

/*******************************************************************************
    Received confirm response callback
*******************************************************************************/
void receiveConfirmSysexCallback(double delta_time, Bytes* message, void* user_data)
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
    Debug::addProcessedHistory(false, synth_conn.input_port_name, *message);
#endif
}

/*******************************************************************************
    Received global dump response callback
*******************************************************************************/
// DSI: Streichfett
void receiveGlobalDumpSysexCallback(double delta_time, Bytes* message, void* user_data)
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

            Bytes global_data =
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
    Debug::addProcessedHistory(false, synth_conn.input_port_name, *message);
#endif
}

/*******************************************************************************
    Received sound dump response callback
*******************************************************************************/
// DSI: Streichfett
void receiveSoundDumpSysexCallback(double delta_time, Bytes* message, void* user_data)
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

            Bytes sound_data =
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
    Debug::addProcessedHistory(false, synth_conn.input_port_name, *message);
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
void receiveKeyDeviceMessageCallback(double delta_time, Bytes* message, void* user_data)
{
    if (isSynthConnected() &&
        (MessageHandler::isNoteOff(*message) || MessageHandler::isNoteOn(*message)))
    {
        if (Connector::force_adjust_midi_channel)
        {
            const int ch = InternalSetting::getDeviceMidiChannel();
            Bytes channel_adj_message;
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

} // Connector
} // StreichfettSse
