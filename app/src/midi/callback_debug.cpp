#include "common.hpp"
#ifdef _DEBUG
#include "midi/midi_common.hpp"
#include "midi/connector.hpp"
#include "midi/connector_debug.hpp"
#include "midi/message_handler.hpp"

namespace StreichfettSse
{
namespace Midi
{
namespace Callback
{
namespace Debug
{

void receiveTestSysex(double delta_time, ByteVec* message, void* user_data)
{
    namespace cd = Connector::Debug;

    auto* type_ptr = static_cast<cd::SendTestType*>(user_data);
    auto type = *type_ptr;

    if (message->empty())
    {
        cd::send_test.at(type) = cd::SendTestResult::Failed;
        cd::send_test_failed_cause.at(type) = cd::SendTestFailedCause::EmptyResponse;
    }
    else
    {
        switch (*type_ptr)
        {
            case cd::SendTestType::DeviceInquiry:
                try
                {
                    MessageHandler::validateInquiryDump(*message);
                    cd::send_test.at(type) = cd::SendTestResult::Ok;
                    cd::send_test_failed_cause.at(type) = cd::SendTestFailedCause::None;
                }
                catch (std::exception&)
                {
                    cd::send_test.at(type) = cd::SendTestResult::Failed;
                    cd::send_test_failed_cause.at(type) = cd::SendTestFailedCause::IncorrectMessage;
                }
                break;
            case cd::SendTestType::Global:
                try
                {
                    MessageHandler::validateDataDump(*message, MessageHandler::DumpType::Global);
                    cd::send_test.at(type) = cd::SendTestResult::Ok;
                    cd::send_test_failed_cause.at(type) = cd::SendTestFailedCause::None;
                }
                catch (std::exception&)
                {
                    cd::send_test.at(type) = cd::SendTestResult::Failed;
                    cd::send_test_failed_cause.at(type) = cd::SendTestFailedCause::IncorrectMessage;
                }
                break;
            case cd::SendTestType::Sound:
                try
                {
                    MessageHandler::validateDataDump(*message, MessageHandler::DumpType::Sound);
                    cd::send_test.at(type) = cd::SendTestResult::Ok;
                    cd::send_test_failed_cause.at(type) = cd::SendTestFailedCause::None;
                }
                catch (std::exception&)
                {
                    cd::send_test.at(type) = cd::SendTestResult::Failed;
                    cd::send_test_failed_cause.at(type) = cd::SendTestFailedCause::IncorrectMessage;
                }
                break;
        }
    }

    Connector::synth_input.cancelCallback();
    SDL_RemoveTimer(waiting_timer);

    delete type_ptr;
    type_ptr = nullptr;

    cd::addProcessedHistory(false, Connector::synth_input.getPortName(), *message);
}

Uint32 timeoutTest(Uint32 interval, void* param)
{
    namespace cd = Connector::Debug;

    auto* type_ptr = static_cast<cd::SendTestType*>(param);
    auto type = *type_ptr;

    Connector::synth_input.cancelCallback();
    SDL_RemoveTimer(waiting_timer);
    cd::send_test.at(type) = cd::SendTestResult::Failed;
    cd::send_test_failed_cause.at(type) = cd::SendTestFailedCause::RequestTimeout;

    delete type_ptr;
    type_ptr = nullptr;

    return interval;
}

} // Debug
} // Callback
} // Midi
} // StreichfettSse
#endif