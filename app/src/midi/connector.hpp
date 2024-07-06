#pragma once
#include "midi/connection.hpp"

namespace StreichfettSse
{
namespace Connector
{

extern InputConnection synth_input;
extern OutputConnection synth_output;
extern InputConnection key_input;
extern std::vector<std::string> in_name_list;
extern std::vector<std::string> out_name_list;
extern bool force_adjust_midi_channel;
extern int store_delay_duration;

void initialize();
void finalize() noexcept;
void applyConfig();
void updateConfig() noexcept;
void resetAllConnections();
void openSynthInputPort(int port_index, const std::string& port_name);
void openSynthOutputPort(int port_index, const std::string& port_name);
void openKeyInputPort(int port_index, const std::string& port_name);
void requestInquiry();
void requestGlobalData();
void requestSoundData();
void sendSoundDump(bool is_edit_buffer = false);
void sendProgChange();
void sendAllSoundOff();
void sendOneTaskMessage();
bool isSynthConnected() noexcept;
void setSynthConnected(bool connected) noexcept;

} // Connector
} // StreichfettSse
