#ifdef _DEBUG
#pragma once

namespace StreichfettSse
{
namespace Connector
{
namespace Debug
{

enum class SendTestType : int
{
    Inquiry,
    GlobalDump,
    SoundDump,
    _COUNT_,
};

enum class SendTestResult : int
{
    NotStarted,
    WaitReceive,
    Ok,
    Failed,
    _COUNT_,
};

enum class SendTestFailedCause : int
{
    RequestTimeout,
    EmptyResponse,
    IncorrectMessage,
    _COUNT_,
};

extern SendTestResult send_test[static_cast<int>(SendTestType::_COUNT_)];
extern SendTestFailedCause send_test_failed_cause[static_cast<int>(SendTestType::_COUNT_)];

struct ProcessedMidiMessage
{
    std::string timestamp;
    bool transmitted;               // true: transmitted, false: received
    std::string device_name;
    std::string description;
    ByteVec data;
    std::string list_title;

    ProcessedMidiMessage()
    {
        timestamp = "";
        transmitted = true;
        device_name = "";
        description = "";
        data = ByteVec();
        list_title = "";
    }

    ProcessedMidiMessage(
        const std::string& ts,
        const bool t,
        const std::string& d_name,
        const std::string& desc,
        const ByteVec d)
    {
        timestamp = ts;
        transmitted = t;
        device_name = d_name;
        description = desc;
        data.clear();
        data = d;

        list_title = format("%s %s %s",
            transmitted ? "T" : "R",
            timestamp.c_str(),
            description.c_str());
    }
};
extern std::list<ProcessedMidiMessage> processed_history;
extern int history_selected_index;
extern ProcessedMidiMessage selected_processed_message;

void addProcessedHistory(const bool transmitted, const std::string& device_name, const ByteVec& data);
void sendTest(const SendTestType type);
void sendTest(SendTestType type);
bool isAnyTestSending() noexcept;

} // Debug
} // Connector
} // StreichfettSse
#endif
