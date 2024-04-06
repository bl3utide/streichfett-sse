#include "common.hpp"
#ifdef _DEBUG
#include "midi/midi_common.hpp"
#include "midi/connector.hpp"
#include "midi/callback_debug.hpp"
#include "midi/connector_debug.hpp"
#include "midi/message_handler.hpp"

namespace StreichfettSse
{
namespace Connector
{
namespace Debug
{
namespace Callback
{

void receiveTestSysex(double delta_time, ByteVec* message, void* user_data)
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

    synth_input.cancelCallback();
    SDL_RemoveTimer(_waiting_timer);

    delete type_ptr;
    type_ptr = nullptr;

    addProcessedHistory(false, synth_input.getPortName(), *message);
}

Uint32 timeoutTest(Uint32 interval, void* param)
{
    SendTestType* type_ptr = static_cast<SendTestType*>(param);
    int type_index = static_cast<int>(*type_ptr);

    synth_input.cancelCallback();
    SDL_RemoveTimer(_waiting_timer);
    send_test[type_index] = SendTestResult::Failed;
    send_test_failed_cause[type_index] = SendTestFailedCause::RequestTimeout;

    delete type_ptr;
    type_ptr = nullptr;

    return interval;
}

} // Callback
} // Debug
} // Connector
} // StreichfettSse
#endif
