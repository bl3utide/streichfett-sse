#pragma once

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

void initialize();
void finalize() noexcept;
void resetAllConnections();
void checkOpenPorts();
void keyOpenPort();
void requestInquiry();
void requestGlobalData();
void requestSoundData();
void sendSoundDump(const bool is_edit_buffer = false);
void sendProgChange();
void sendAllSoundOff();
void sendOneTaskMessage();
bool isSynthConnected() noexcept;
void setSynthConnected(const bool connected) noexcept;

} // Connector
} // StreichfettSse
