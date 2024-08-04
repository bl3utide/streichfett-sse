#include "common.hpp"
#ifdef _DEBUG
#include "midi/midi_common.hpp"
#include "midi/connector.hpp"
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
    auto* type_ptr = static_cast<SendTestType*>(user_data);
    auto type = *type_ptr;

    if (message->empty())
    {
        send_test.at(type) = SendTestResult::Failed;
        send_test_failed_cause.at(type) = SendTestFailedCause::EmptyResponse;
    }
    else
    {
        switch (*type_ptr)
        {
            case SendTestType::Inquiry:
                if (MessageHandler::checkInquiryDump(*message))
                {
                    send_test.at(type) = SendTestResult::Ok;
                    send_test_failed_cause.at(type) = SendTestFailedCause::None;
                }
                else
                {
                    send_test.at(type) = SendTestResult::Failed;
                    send_test_failed_cause.at(type) = SendTestFailedCause::IncorrectMessage;
                }
                break;
            case SendTestType::GlobalDump:
                try
                {
                    MessageHandler::checkDump(*message, MessageHandler::DumpType::Global);
                    send_test.at(type) = SendTestResult::Ok;
                    send_test_failed_cause.at(type) = SendTestFailedCause::None;
                }
                catch (std::exception&)
                {
                    send_test.at(type) = SendTestResult::Failed;
                    send_test_failed_cause.at(type) = SendTestFailedCause::IncorrectMessage;
                }
                break;
            case SendTestType::SoundDump:
                try
                {
                    MessageHandler::checkDump(*message, MessageHandler::DumpType::Sound);
                    send_test.at(type) = SendTestResult::Ok;
                    send_test_failed_cause.at(type) = SendTestFailedCause::None;
                }
                catch (std::exception&)
                {
                    send_test.at(type) = SendTestResult::Failed;
                    send_test_failed_cause.at(type) = SendTestFailedCause::IncorrectMessage;
                }
                break;
        }
    }

    synth_input.cancelCallback();
    SDL_RemoveTimer(waiting_timer);

    delete type_ptr;
    type_ptr = nullptr;

    addProcessedHistory(false, synth_input.getPortName(), *message);
}

Uint32 timeoutTest(Uint32 interval, void* param)
{
    auto* type_ptr = static_cast<SendTestType*>(param);
    auto type = *type_ptr;

    synth_input.cancelCallback();
    SDL_RemoveTimer(waiting_timer);
    send_test.at(type) = SendTestResult::Failed;
    send_test_failed_cause.at(type) = SendTestFailedCause::RequestTimeout;

    delete type_ptr;
    type_ptr = nullptr;

    return interval;
}

} // Callback
} // Debug
} // Connector
} // StreichfettSse
#endif
