#include "common.hpp"
#include "annotation.hpp"
#include "error.hpp"
#include "logger.hpp"
#include "operation.hpp"
#include "state.hpp"
#include "data/internal_patch.hpp"
#include "data/internal_setting.hpp"
#include "midi/midi_common.hpp"
#include "midi/callback.hpp"
#include "midi/connector.hpp"
#include "midi/message_handler.hpp"
#ifdef _DEBUG
#include "midi/connector_debug.hpp"
#endif

namespace StreichfettSse
{
namespace Connector
{
namespace Callback
{

/*******************************************************************************
    Received confirm response callback
*******************************************************************************/
void receiveConfirmSysex(double delta_time, ByteVec* message, void* user_data)
{
    if (message->empty())
    {
        Logger::debug("received empty confirm dump");
        setAppError("Incorrect confirm dump");
        setSynthConnected(false);
    }
    else
    {
        if (MessageHandler::checkInquiryDump(*message))
        {
            //Annotation::clearText();  // TODO delete setSynthConnected(true)の時にやるので不要
            setNextState(State::RequestGlobal, true);
            //setSynthConnected(true);  // TODO delete この時点での判定は行わないことにした
        }
        else
        {
            const auto byte_str = MessageHandler::getByteVecString(*message);
            Logger::debug("checkInquiryDump failed");
            Logger::debug(StringUtil::format(" >> %s", byte_str.c_str()).c_str());
            setAppError("You tried to connect to an incorrect device.");
            setNextState(State::Idle, true);
            setSynthConnected(false);
        }
    }

    synth_input.cancelCallback();
    SDL_RemoveTimer(_waiting_timer);

#ifdef _DEBUG
    Debug::addProcessedHistory(false, synth_input.getPortName(), *message);
#endif
}

/*******************************************************************************
    Received global dump response callback
*******************************************************************************/
// DSI: Streichfett
void receiveGlobalDumpSysex(double delta_time, ByteVec* message, void* user_data)
{
    if (message->empty())
    {
        Logger::debug("received empty global dump");
        setAppError("Incorrect global dump");
        setSynthConnected(false);
    }
    else
    {
        MessageHandler::DumpType dump_type = MessageHandler::DumpType::Global;

        try
        {
            // throwable function
            MessageHandler::checkDump(*message, dump_type);

            ByteVec global_data =
                MessageHandler::getDataBytesFromDump(*message, dump_type);
            GlobalModel::Global* setting = InternalSetting::getGlobalData();

            // throwable function
            InternalSetting::setSettingFromBytes(setting, global_data);

            Operation op = getOperation();
            if (op == Operation::Sound)
                setNextState(State::SendBankProgChange, true);
            else if (op == Operation::Option)
                setNextState(State::Idle, true);
        }
        catch (std::exception& e)
        {
            Logger::debug(e.what());
            setAppError("Incorrect global dump message");
            setNextState(State::Idle, true);
            setSynthConnected(false);
        }
    }

    synth_input.cancelCallback();
    SDL_RemoveTimer(_waiting_timer);

#ifdef _DEBUG
    Debug::addProcessedHistory(false, synth_input.getPortName(), *message);
#endif
}

/*******************************************************************************
    Received sound dump response callback
*******************************************************************************/
// DSI: Streichfett
void receiveSoundDumpSysex(double delta_time, ByteVec* message, void* user_data)
{
    if (message->empty())
    {
        Logger::debug("received empty sound dump");
        setAppError("Incorrect sound dump");
    }
    else
    {
        MessageHandler::DumpType dump_type = MessageHandler::DumpType::Sound;

        try
        {
            // throwable function
            MessageHandler::checkDump(*message, dump_type);

            ByteVec sound_data =
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

            setNextState(State::Idle, true);
        }
        catch (std::exception& e)
        {
            Logger::debug(e.what());
            setAppError("Incorrect sound dump message");
        }
    }

    synth_input.cancelCallback();
    SDL_RemoveTimer(_waiting_timer);

#ifdef _DEBUG
    Debug::addProcessedHistory(false, synth_input.getPortName(), *message);
#endif
}

/*******************************************************************************
    Timeout response callback (General purpose)
*******************************************************************************/
Uint32 timeout(Uint32 interval, void* param)
{
    std::string err_message = *static_cast<std::string*>(param);

    synth_input.cancelCallback();
    SDL_RemoveTimer(_waiting_timer);
    setAppError(err_message);
    setNextState(State::Idle, true);
    setSynthConnected(false);

    return interval;
}

/*******************************************************************************
    Received message from key-device callback
*******************************************************************************/
// DSI: Streichfett
void receiveKeyDeviceMessage(double delta_time, ByteVec* message, void* user_data)
{
    if (isSynthConnected() &&
        (MessageHandler::isNoteOff(*message) || MessageHandler::isNoteOn(*message)))
    {
        ByteVec send_message;
        if (Connector::force_adjust_midi_channel)
        {
            const int ch = InternalSetting::getDeviceMidiChannel();
            if (MessageHandler::isNoteOff(*message))
            {
                send_message = {
                    static_cast<Byte>(0x80 + ch),
                    message->at(1),
                    message->at(2)
                };
            }
            else
            {
                send_message = {
                    static_cast<Byte>(0x90 + ch),
                    message->at(1),
                    message->at(2)
                };
            }
        }
        else
        {
            send_message = *message;
        }

        try
        {
            synth_output.sendMessage(&send_message);
        }
        catch (RtMidiError& error)
        {
            Logger::debug(StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str());
            setAppError("MIDI error when sending message from key to output");
        }
    }
}

/*******************************************************************************
    Store delay callback
*******************************************************************************/
// DSI: Streichfett
Uint32 storeDelay(Uint32 interval, void* param)
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

Uint32 sendDelay(Uint32 interval, void* param)
{
    State* next_state_ptr = static_cast<State*>(param);

    setNextState(*next_state_ptr, true);

    SDL_RemoveTimer(_waiting_timer);

    delete next_state_ptr;
    next_state_ptr = nullptr;

    return interval;
}

} // Callback
} // Connector
} // StreichfettSse
