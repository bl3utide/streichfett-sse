#include "common.hpp"
#include "annotation.hpp"
#include "error.hpp"
#include "logger.hpp"
#include "operation.hpp"
#include "state.hpp"
#include "data/local_patch.hpp"
#include "data/local_setting.hpp"
#include "midi/midi_common.hpp"
#include "midi/connector.hpp"
#include "midi/device_inquiry_info.h"
#include "midi/message_entity/data_dump_picker.h"
#include "midi/message_entity/dump_validator.h"
#include "midi/message_entity/message_stringizer.h"
#include "midi/message_creator/channel_message_creator.h"
#ifdef _DEBUG
#include "midi/connector_debug.hpp"
#endif

namespace StreichfettSse
{
namespace Midi
{
namespace Callback
{

enum class ErrorMessageType : int
{
    Timeout,
    Incorrect,
    _COUNT_,
};

// private
const std::unordered_map<RequestType, std::unordered_map<ErrorMessageType, std::string>> ERROR_MESSAGE
{
    {
        RequestType::DeviceInquiry,
        {
            { ErrorMessageType::Timeout,    "The device inquiry request has timed out." },
            { ErrorMessageType::Incorrect,  "You tried to connect to an incorrect device." },
        }
    },
    {
        RequestType::Global,
        {
            { ErrorMessageType::Timeout,    "The global request has timed out." },
            { ErrorMessageType::Incorrect,  "Incorrect global dump message." },
        }
    },
    {
        RequestType::Sound,
        {
            { ErrorMessageType::Timeout,    "The sound request has timed out." },
            { ErrorMessageType::Incorrect,  "Incorrect sound dump message." },
        }
    },
};

static void requestSuccessful(State next_state) noexcept
{
    setNextState(next_state, true);
    request_try_count.reset();
}

static void retryOver(
    RequestType req_type,
    ErrorMessageType mes_type,
    bool set_disconnected = true) noexcept
{
    setAppError(ERROR_MESSAGE.at(req_type).at(mes_type));
    setNextState(State::Idle, true);

    if (set_disconnected) Connector::setSynthConnected(false);

    request_try_count.reset();
}

/*******************************************************************************
    Received device inquiry response callback
*******************************************************************************/
void receiveDeviceInquiryDump(double delta_time, ByteVec* message, void* user_data)
{
    Logger::debug("callback function exec: receiveDeviceInquiryDump");
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
            Connector::synth_input.cancelCallback();

            try
            {
                // throwable function
                DeviceInquiryDumpValidator{ *message }.validate();

                // apply the result of inquiry dump locally
                inquiry_info = DeviceInquiryInfo::makeFromMessage(*message);

                // received correct dump
                Logger::debug("received correct inquiry dump");
                requestSuccessful(State::RequestGlobal);
            }
            catch (std::exception& e)
            {
                Logger::debug(e.what());

                const auto byte_str = MessageStringizer{ *message }.toString();
                Logger::debug(std::format(" >> {}", byte_str));

                if (request_try_count.reachedLimit())
                {
                    // retry count over
                    Logger::debug("request count over, the last cause: received incorrect dump");
                    retryOver(RequestType::DeviceInquiry, ErrorMessageType::Incorrect);
                }
                else
                {
                    // retry
                    Logger::debug("retry -> request inquiry");
                    setNextState(State::RequestDeviceInquiry, true);
                }
            }
#ifdef _DEBUG
            Connector::Debug::addProcessedHistory(false, Connector::synth_input.getPortName(), *message);
#endif
        }
        else
        {
            Logger::debug("receive device inquiry dump callback can lock but the other callback already executed");
        }

        callback_mutex.guard.unlock();
    }
    else
    {
        // cannot lock
        Logger::debug("receive device inquiry dump callback can not execute the process because of mutex");
    }
}

/*******************************************************************************
    Received global dump response callback
*******************************************************************************/
// DSI: Streichfett
void receiveGlobalDump(double delta_time, ByteVec* message, void* user_data)
{
    Logger::debug("callback function exec: receiveGlobalDump");
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
            Connector::synth_input.cancelCallback();

            try
            {
                // throwable
                GlobalDumpValidator{ *message }.validate();
                const auto global_data = GlobalDumpPicker{ *message }.getDataBytes();

                auto& setting = LocalSetting::getGlobalData();

                // throwable function
                LocalSetting::setSettingFromBytes(setting, global_data);

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

                const auto byte_str = MessageStringizer{ *message }.toString();
                Logger::debug(std::format(" >> {}", byte_str));

                if (request_try_count.reachedLimit())
                {
                    // retry count over
                    Logger::debug("request count over, the last cause: received incorrect dump");
                    retryOver(RequestType::Global, ErrorMessageType::Incorrect);
                }
                else
                {
                    // retry
                    Logger::debug("retry -> request global");
                    setNextState(State::RequestGlobal, true);
                }
            }
#ifdef _DEBUG
            Connector::Debug::addProcessedHistory(false, Connector::synth_input.getPortName(), *message);
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
void receiveSoundDump(double delta_time, ByteVec* message, void* user_data)
{
    Logger::debug("callback function exec: receiveSoundDump");
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
            Connector::synth_input.cancelCallback();

            try
            {
                // throwable
                SoundDumpValidator{ *message }.validate();
                const auto sound_data = SoundDumpPicker{ *message }.getDataBytes();

                auto& original = LocalPatch::getOriginalPatch();
                auto& current = LocalPatch::getCurrentPatch();

                // throwable function
                LocalPatch::setPatchFromBytes(original, sound_data);
                LocalPatch::copyPatchAtoB(original, current);

                // received correct dump
                Logger::debug("received correct sound dump");
                requestSuccessful(State::Idle);

                auto& patch_addr = LocalPatch::getCurrentPatchAddress();
                const auto bb = LocalPatch::getPatchBankChar(patch_addr);
                const auto bs = LocalPatch::getPatchSoundNumber(patch_addr);
                char buf[64];
                sprintf(buf, "Sound %c%d loaded.", bb, bs);
                Annotation::setText(buf, Annotation::Type::General);
            }
            catch (std::exception& e)
            {
                Logger::debug(e.what());

                const auto byte_str = MessageStringizer{ *message }.toString();
                Logger::debug(std::format(" >> {}", byte_str));

                if (request_try_count.reachedLimit())
                {
                    // retry count over
                    Logger::debug("request count over, the last cause: received incorrect dump");
                    retryOver(RequestType::Sound, ErrorMessageType::Incorrect, false);
                }
                else
                {
                    // retry
                    Logger::debug("retry -> request sound");
                    setNextState(State::RequestSound, true);
                }
            }
#ifdef _DEBUG
            Connector::Debug::addProcessedHistory(false, Connector::synth_input.getPortName(), *message);
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
    Logger::debug("callback function exec: timeout");
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
            Connector::synth_input.cancelCallback();

            if (request_try_count.reachedLimit())
            {
                // retry count over
                Logger::debug("request count over, the last cause: request timeout");
                retryOver(req_type, ErrorMessageType::Timeout);
            }
            else
            {
                // retry
                if (req_type == RequestType::DeviceInquiry)
                {
                    Logger::debug("retry -> request inquiry");
                    setNextState(State::RequestDeviceInquiry, true);
                }
                else if (req_type == RequestType::Global)
                {
                    Logger::debug("retry -> request global");
                    setNextState(State::RequestGlobal, true);
                }
                else if (req_type == RequestType::Sound)
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
    const auto type = MessageEntity{ *message }.type();
    if (Connector::isSynthConnected() &&
        (type == MessageType::NoteOff || type == MessageType::NoteOn))
    {
        ByteVec send_message;
        if (Connector::force_adjust_midi_channel)
        {
            const auto ch = LocalSetting::getDeviceMidiChannel();
            const auto note = static_cast<int>(message->at(1));
            const auto velocity = static_cast<int>(message->at(2));
            if (type == MessageType::NoteOff)
            {
                send_message = NoteOffCreator{ ch, note, velocity }.create();
            }
            else
            {
                send_message = NoteOnCreator{ ch, note, velocity }.create();
            }
        }
        else
        {
            send_message = *message;
        }

        try
        {
            Connector::synth_output.sendMessage(send_message);
        }
        catch (RtMidiError& error)
        {
            Logger::debug(std::format("MIDI error: {}", error.getMessage()));
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
    auto patch_address_ptr = static_cast<LocalPatch::PatchAddress*>(param);

    int sound = patch_address_ptr->sound;

    // -1: sent to edit buffer
    if (sound != -1)
    {
        auto& original = LocalPatch::getOriginalPatch();
        auto& current = LocalPatch::getCurrentPatch();
        LocalPatch::copyPatchAtoB(current, original);

        const auto bb = LocalPatch::getPatchBankChar(sound);
        const auto bs = LocalPatch::getPatchSoundNumber(sound);
        char buf[64];
        sprintf(buf, "The sound has stored to %c%d", bb, bs);
        Annotation::setText(buf, Annotation::Type::General);
        LocalPatch::current_patch_changed = false;
    }

    SDL_RemoveTimer(waiting_timer);
    setWaitingStoreDelay(false);

    delete patch_address_ptr;
    patch_address_ptr = nullptr;

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
} // Midi
} // StreichfettSse
