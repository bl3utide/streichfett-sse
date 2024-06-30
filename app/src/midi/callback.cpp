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
    if (callback_mutex.guard.try_lock())
    {
        if (!callback_mutex.is_callback_catched)
        {
            // succeed
            RequestType* req_type_ptr = static_cast<RequestType*>(user_data);
            delete req_type_ptr;
            req_type_ptr = nullptr;

            callback_mutex.is_callback_catched = true;
            SDL_RemoveTimer(_waiting_timer);
            synth_input.cancelCallback();

            request_try_count = 0;

            if (message->empty())
            {
                Logger::debug("received empty confirm dump");
                setAppError("Incorrect confirm dump");
                setNextState(State::Idle, true);
                setSynthConnected(false);
            }
            else
            {
                if (MessageHandler::checkInquiryDump(*message))
                {
                    setNextState(State::RequestGlobal, true);
                }
                else
                {
                    const auto byte_str = MessageHandler::getByteVecString(*message);
                    Logger::debug("checkInquiryDump failed");
                    Logger::debug(StringUtil::format(" >> %s", byte_str.c_str()));
                    setAppError("You tried to connect to an incorrect device.");
                    setNextState(State::Idle, true);
                    setSynthConnected(false);
                }
            }
#ifdef _DEBUG
            Debug::addProcessedHistory(false, synth_input.getPortName(), *message);
#endif
        }
        else
        {
            Logger::debug("receive confirm callback can lock but the other callback already executed");
        }

        callback_mutex.guard.unlock();
    }
    else
    {
        // cannot lock
        Logger::debug("receive confirm callback can not execute the process because of mutex");
    }
}

/*******************************************************************************
    Received global dump response callback
*******************************************************************************/
// DSI: Streichfett
void receiveGlobalDumpSysex(double delta_time, ByteVec* message, void* user_data)
{
    if (callback_mutex.guard.try_lock())
    {
        if (!callback_mutex.is_callback_catched)
        {
            // succeed
            RequestType* req_type_ptr = static_cast<RequestType*>(user_data);
            delete req_type_ptr;
            req_type_ptr = nullptr;

            callback_mutex.is_callback_catched = true;
            SDL_RemoveTimer(_waiting_timer);
            synth_input.cancelCallback();

            request_try_count = 0;

            if (message->empty())
            {
                Logger::debug("received empty global dump");
                setAppError("Incorrect global dump");
                setNextState(State::Idle, true);
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
#ifdef _DEBUG
            Debug::addProcessedHistory(false, synth_input.getPortName(), *message);
#endif
        }
        else
        {
            Logger::debug("receive global dump callback can lock but the other callback already executed");
        }

        callback_mutex.guard.unlock();
    }
    else
    {
        // cannot lock
        Logger::debug("receive global dump callback can not execute the process because of mutex");
    }
}

/*******************************************************************************
    Received sound dump response callback
*******************************************************************************/
// DSI: Streichfett
void receiveSoundDumpSysex(double delta_time, ByteVec* message, void* user_data)
{
    if (callback_mutex.guard.try_lock())
    {
        if (!callback_mutex.is_callback_catched)
        {
            // succeed
            RequestType* req_type_ptr = static_cast<RequestType*>(user_data);
            delete req_type_ptr;
            req_type_ptr = nullptr;

            callback_mutex.is_callback_catched = true;
            SDL_RemoveTimer(_waiting_timer);
            synth_input.cancelCallback();

            request_try_count = 0;

            if (message->empty())
            {
                Logger::debug("received empty sound dump");
                setAppError("Incorrect sound dump");
                setNextState(State::Idle, true);
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
                    setNextState(State::Idle, true);
                }
            }
#ifdef _DEBUG
            Debug::addProcessedHistory(false, synth_input.getPortName(), *message);
#endif
        }
        else
        {
            Logger::debug("receive sound dump callback can lock but the other callback already executed");
        }

        callback_mutex.guard.unlock();
    }
    else
    {
        // cannot lock
        Logger::debug("receive sound dump callback can not execute the process because of mutex");
    }
}

/*******************************************************************************
    Timeout response callback (General purpose)
*******************************************************************************/
Uint32 timeout(Uint32 interval, void* param)
{
    //std::string err_message = *static_cast<std::string*>(param);
    if (callback_mutex.guard.try_lock())
    {
        if (!callback_mutex.is_callback_catched)
        {
            // succeed
            RequestType* req_type_ptr = static_cast<RequestType*>(param);
            RequestType req_type = *req_type_ptr;
            delete req_type_ptr;
            req_type_ptr = nullptr;

            callback_mutex.is_callback_catched = true;
            SDL_RemoveTimer(_waiting_timer);
            synth_input.cancelCallback();

            if (request_try_count < MAX_REQUEST_TRY)
            {
                // retry
                if (req_type == RequestType::Confirm)
                    setNextState(State::RequestInquiry);
                else if (req_type == RequestType::GlobalDump)
                    setNextState(State::RequestGlobal);
                else if (req_type == RequestType::SoundDump)
                    setNextState(State::RequestSound);
            }
            else
            {
                // failed due because of retry count over
                setAppError(ERROR_MESSAGE_TIMEOUT[static_cast<int>(req_type)]);
                setNextState(State::Idle, true);
                setSynthConnected(false);

                request_try_count = 0;
            }
        }
        else
        {
            Logger::debug("timeout callback can lock but the other callback already executed");
        }

        callback_mutex.guard.unlock();
    }
    else
    {
        // cannot lock
        Logger::debug("timeout callback can not execute the process because of mutex");
    }

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
            Logger::debug(StringUtil::format("MIDI error: %s", error.getMessage().c_str()));
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
