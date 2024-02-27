#pragma once

#include "midi/message_handler.hpp"
#include "util/string_util.hpp"

namespace StreichfettSse
{
namespace Connector
{

struct MidiConnection
{
    RtMidiIn* input = nullptr;
    RtMidiOut* output = nullptr;
    int input_port_index = -1;
    int output_port_index = -1;
    std::string input_port_name;
    std::string output_port_name;

    void initialize()
    {
        input = new RtMidiIn();
        output = new RtMidiOut();
    }

    void finalize()
    {
        input->cancelCallback();
        input->closePort();
        if (input != nullptr)
        {
            delete input;
            input = nullptr;
        }

        output->closePort();
        if (output != nullptr)
        {
            delete output;
            output = nullptr;
        }
    }

    void openPorts()
    {
        input->cancelCallback();
        input->closePort();
        input->openPort(input_port_index);

        output->closePort();
        output->openPort(output_port_index);
    }

    void openInPort()
    {
        input->cancelCallback();
        input->closePort();
        input->openPort(input_port_index);
    }

    void closePorts()
    {
        input->cancelCallback();
        input->closePort();
        output->closePort();
    }

    void resetPortInfo()
    {
        input_port_index = -1;
        input_port_name = "";
        output_port_index = -1;
        output_port_name = "";
    }

    void operator=(const MidiConnection& conn)
    {
        input_port_index = conn.input_port_index;
        input_port_name = conn.input_port_name;
        output_port_index = conn.output_port_index;
        output_port_name = conn.output_port_name;
    }
};

extern MidiConnection synth_conn;
extern MidiConnection key_conn;
extern std::vector<std::string> in_name_list;
extern std::vector<std::string> out_name_list;
extern bool force_adjust_midi_channel;
extern int store_delay_duration;

#ifdef _DEBUG
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
    MessageHandler::Bytes data;
    std::string list_title;

    ProcessedMidiMessage()
    {
        timestamp = "";
        transmitted = true;
        device_name = "";
        description = "";
        data = MessageHandler::Bytes();
        list_title = "";
    }

    ProcessedMidiMessage(
        const std::string& ts,
        const bool t,
        const std::string& d_name,
        const std::string& desc,
        const MessageHandler::Bytes d)
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
#endif

void initialize();
void finalize() noexcept;
void resetAllConnections();
#ifdef _DEBUG
void sendTest(SendTestType type);
bool isAnyTestSending() noexcept;
#endif
void checkOpenPorts();
void keyOpenPort();
void requestInquiry();
void requestGlobalData();
void requestSoundData();
void sendSoundDump(const bool is_edit_buffer = false);
void sendProgChange();
void sendAllSoundOff();
void sendOneTaskMessage();
bool isWaitingStoreDelay() noexcept;
bool isSynthConnected() noexcept;
void setSynthConnected(const bool connected) noexcept;

} // Connector
} // StreichfettSse
