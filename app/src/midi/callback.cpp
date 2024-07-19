#include "common.hpp"
#include "annotation.hpp"
#include "error.hpp"
#include "logger.hpp"
#include "operation.hpp"
#include "state.hpp"
#include "data/internal_patch.hpp"
#include "data/internal_setting.hpp"
#include "midi/midi_common.hpp"
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

enum class ErrorMessageType : int
{
    Timeout,
    Empty,
    Incorrect,
    _COUNT_,
};

// private
const std::unordered_map<RequestType, std::unordered_map<ErrorMessageType, std::string>> ERROR_MESSAGE
{
    {
        RequestType::Confirm,
        {
            { ErrorMessageType::Timeout,    "The confirm sysex request has timed out." },
            { ErrorMessageType::Empty,      "Received empty confirm dump." },
            { ErrorMessageType::Incorrect,  "You tried to connect to an incorrect device." },
        }
    },
    {
        RequestType::GlobalDump,
        {
            { ErrorMessageType::Timeout,    "The global dump request has timed out." },
            { ErrorMessageType::Empty,      "Received empty global dump."},
            { ErrorMessageType::Incorrect,  "Incorrect global dump message." },
        }
    },
    {
        RequestType::SoundDump,
        {
            { ErrorMessageType::Timeout,    "The sound dump request has timed out." },
            { ErrorMessageType::Empty,      "Received empty sound dump." },
            { ErrorMessageType::Incorrect,  "Incorrect sound dump message." },
        }
    },
};

void requestSuccessful(State next_state) noexcept
{
    setNextState(next_state, true);
    request_try_count.reset();
}

void retryOver(
    RequestType req_type,
    ErrorMessageType mes_type,
    bool set_disconnected = true) noexcept
{
    setAppError(ERROR_MESSAGE.at(req_type).at(mes_type));
    setNextState(State::Idle, true);

    if (set_disconnected) setSynthConnected(false);

    request_try_count.reset();
}

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
            auto req_type_ptr = static_cast<RequestType*>(user_data);
            delete req_type_ptr;
            req_type_ptr = nullptr;

            callback_mutex.is_callback_catched = true;
            SDL_RemoveTimer(waiting_timer);
            synth_input.cancelCallback();

            if (message->empty())
            {
                Logger::debug("received empty confirm dump");

                if (request_try_count.reachedLimit())
                {
                    // retry count over
                    Logger::debug("request count over, the last cause: received empty dump");
                    retryOver(RequestType::Confirm, ErrorMessageType::Empty);
                }
                else
                {
                    // retry
                    Logger::debug("retry -> request inquiry");
                    setNextState(State::RequestInquiry, true);
                }
            }
            else
            {
                if (MessageHandler::checkInquiryDump(*message))
                {
                    // received correct dump
                    Logger::debug("received correct inquiry dump");
                    requestSuccessful(State::RequestGlobal);
                }
                else
                {
                    Logger::debug("checkInquiryDump failed");
                    const auto byte_str = MessageHandler::getByteVecString(*message);
                    Logger::debug(StringUtil::format(" >> %s", byte_str.c_str()));

                    if (request_try_count.reachedLimit())
                    {
                        // retry count over
                        Logger::debug("request count over, the last cause: received incorrect dump");
                        retryOver(RequestType::Confirm, ErrorMessageType::Incorrect);
                    }
                    else
                    {
                        // retry
                        Logger::debug("retry -> request inquiry");
                        setNextState(State::RequestInquiry, true);
                    }
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
            auto req_type_ptr = static_cast<RequestType*>(user_data);
            delete req_type_ptr;
            req_type_ptr = nullptr;

            callback_mutex.is_callback_catched = true;
            SDL_RemoveTimer(waiting_timer);
            synth_input.cancelCallback();

            if (message->empty())
            {
                Logger::debug("received empty global dump");

                if (request_try_count.reachedLimit())
                {
                    // retry count over
                    Logger::debug("request count over, the last cause: received empty dump");
                    retryOver(RequestType::GlobalDump, ErrorMessageType::Empty);
                }
                else
                {
                    // retry
                    Logger::debug("retry -> request global");
                    setNextState(State::RequestGlobal, true);
                }
            }
            else
            {
                const auto dump_type = MessageHandler::DumpType::Global;

                try
                {
                    // throwable function
                    MessageHandler::checkDump(*message, dump_type);

                    const auto global_data = MessageHandler::getDataBytesFromDump(*message, dump_type);
                    auto& setting = InternalSetting::getGlobalData();

                    // throwable function
                    InternalSetting::setSettingFromBytes(setting, global_data);

                    // received correct dump
                    Logger::debug("received correct global dump");
                    const auto op = getOperation();
                    if (op == Operation::Sound)
                    {
                        requestSuccessful(State::SendBankProgChange);
                    }
                    else if (op == Operation::Option)
                    {
                        requestSuccessful(State::Idle);
                    }
                }
                catch (std::exception& e)
                {
                    Logger::debug(e.what());

                    if (request_try_count.reachedLimit())
                    {
                        // retry count over
                        Logger::debug("request count over, the last cause: received incorrect dump");
                        retryOver(RequestType::GlobalDump, ErrorMessageType::Incorrect);
                    }
                    else
                    {
                        // retry
                        Logger::debug("retry -> request global");
                        setNextState(State::RequestGlobal, true);
                    }
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
            auto req_type_ptr = static_cast<RequestType*>(user_data);
            delete req_type_ptr;
            req_type_ptr = nullptr;

            callback_mutex.is_callback_catched = true;
            SDL_RemoveTimer(waiting_timer);
            synth_input.cancelCallback();

            if (message->empty())
            {
                Logger::debug("received empty sound dump");

                if (request_try_count.reachedLimit())
                {
                    // retry count over
                    Logger::debug("request count over, the last cause: received empty dump");
                    retryOver(RequestType::SoundDump, ErrorMessageType::Empty, false);
                }
                else
                {
                    // retry
                    Logger::debug("retry -> request sound");
                    setNextState(State::RequestSound, true);
                }
            }
            else
            {
                const auto dump_type = MessageHandler::DumpType::Sound;

                try
                {
                    // throwable function
                    MessageHandler::checkDump(*message, dump_type);

                    const auto sound_data =
                        MessageHandler::getDataBytesFromDump(*message, dump_type);
                    auto& original = InternalPatch::getOriginalPatch();
                    auto& current = InternalPatch::getCurrentPatch();

                    // throwable function
                    InternalPatch::setPatchFromBytes(original, sound_data);
                    InternalPatch::copyPatchAtoB(original, current);

                    // received correct dump
                    Logger::debug("received correct sound dump");
                    requestSuccessful(State::Idle);

                    auto& sound_addr = InternalPatch::getCurrentSoundAddress();
                    const auto bb = InternalPatch::getSoundBankChar(sound_addr.sound);
                    const auto bs = InternalPatch::getSoundPatchNumber(sound_addr.sound);
                    char buf[64];
                    sprintf(buf, "Sound %c%d loaded.", bb, bs);
                    Annotation::setText(buf, Annotation::Type::General);
                }
                catch (std::exception& e)
                {
                    Logger::debug(e.what());

                    if (request_try_count.reachedLimit())
                    {
                        // retry count over
                        Logger::debug("request count over, the last cause: received incorrect dump");
                        retryOver(RequestType::SoundDump, ErrorMessageType::Incorrect, false);
                    }
                    else
                    {
                        // retry
                        Logger::debug("retry -> request sound");
                        setNextState(State::RequestSound, true);
                    }
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
            auto req_type_ptr = static_cast<RequestType*>(param);
            auto req_type = *req_type_ptr;
            delete req_type_ptr;
            req_type_ptr = nullptr;

            callback_mutex.is_callback_catched = true;
            SDL_RemoveTimer(waiting_timer);
            synth_input.cancelCallback();

            if (request_try_count.reachedLimit())
            {
                // retry count over
                Logger::debug("request count over, the last cause: request timeout");
                retryOver(req_type, ErrorMessageType::Timeout);
            }
            else
            {
                // retry
                if (req_type == RequestType::Confirm)
                {
                    Logger::debug("retry -> request inquiry");
                    setNextState(State::RequestInquiry, true);
                }
                else if (req_type == RequestType::GlobalDump)
                {
                    Logger::debug("retry -> request global");
                    setNextState(State::RequestGlobal, true);
                }
                else if (req_type == RequestType::SoundDump)
                {
                    Logger::debug("retry -> request sound");
                    setNextState(State::RequestSound, true);
                }
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
            const auto ch = InternalSetting::getDeviceMidiChannel();
            if (MessageHandler::isNoteOff(*message))
            {
                send_message =
                {
                    static_cast<Byte>(0x80 + ch),
                    message->at(1),
                    message->at(2)
                };
            }
            else
            {
                send_message =
                {
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
            synth_output.sendMessage(send_message);
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
        auto& original = InternalPatch::getOriginalPatch();
        auto& current = InternalPatch::getCurrentPatch();
        InternalPatch::copyPatchAtoB(current, original);

        const auto bb = InternalPatch::getSoundBankChar(sound);
        const auto bs = InternalPatch::getSoundPatchNumber(sound);
        char buf[64];
        sprintf(buf, "The sound has stored to %c%d", bb, bs);
        Annotation::setText(buf, Annotation::Type::General);
        InternalPatch::current_patch_changed = false;
    }

    SDL_RemoveTimer(waiting_timer);
    setWaitingStoreDelay(false);

    delete sound_address_ptr;
    sound_address_ptr = nullptr;

    return interval;
}

Uint32 sendDelay(Uint32 interval, void* param)
{
    auto next_state_ptr = static_cast<State*>(param);

    setNextState(*next_state_ptr, true);

    SDL_RemoveTimer(waiting_timer);

    delete next_state_ptr;
    next_state_ptr = nullptr;

    return interval;
}

} // Callback
} // Connector
} // StreichfettSse
