#include "common.hpp"
#include "midi/connection.hpp"

namespace StreichfettSse
{
namespace Midi
{

Connection::Connection()
{
    resetPortInfo();
}

Connection::~Connection()
{
}

void Connection::open(int port_index, const std::string& port_name)
{
    close();

    try
    {
        port_index_ = port_index;
        port_name_ = port_name;
        rtmidi->openPort(port_index);
        setLastConnectedPortIndex(port_index);
    }
    catch (RtMidiError&)
    {
        setLastFailedPortIndex(port_index);
        throw;
    }
}

void Connection::close() noexcept
{
    rtmidi->closePort();
    resetPortInfo();
}

bool Connection::isPortOpen() const
{
    return rtmidi->isPortOpen();
}

unsigned int Connection::getPortCount() const
{
    return rtmidi->getPortCount();
}

const std::string Connection::getPortName(unsigned int port_index) const
{
    // NOTE: Since rtmidi->getPortName returns a non ref-value,
    //       this function should return the copy value.
    return rtmidi->getPortName(port_index);
}

int Connection::getPortIndex() const noexcept
{
    return port_index_;
}

const std::string& Connection::getPortName() const noexcept
{
    return port_name_;
}

int Connection::getLastConnectedPortIndex() const noexcept
{
    return last_connected_port_index_;
}

int Connection::getLastFailedPortIndex() const noexcept
{
    return last_failed_port_index_;
}

void Connection::setLastConnectedPortIndex(int port_index) noexcept
{
    last_connected_port_index_ = port_index;
    last_failed_port_index_ = -1;
}

void Connection::setLastFailedPortIndex(int port_index) noexcept
{
    last_failed_port_index_ = port_index;
    last_connected_port_index_ = -1;
}

void Connection::resetPortInfo() noexcept
{
    port_index_ = -1;
    port_name_ = "";
    last_connected_port_index_ = -1;
    last_failed_port_index_ = -1;
}

InputConnection::InputConnection() : Connection()
{
}

void InputConnection::initialize()
{
    rtmidi = new RtMidiIn();
}

void InputConnection::finalize() noexcept
{
    if (rtmidi != nullptr)
    {
        close();
        delete dynamic_cast<RtMidiIn*>(rtmidi);
        rtmidi = nullptr;
    }
}

void InputConnection::close() noexcept
{
    cancelCallback();
    Connection::close();
}

void InputConnection::setCallback(RtMidiIn::RtMidiCallback callback, void* user_data)
{
    dynamic_cast<RtMidiIn*>(rtmidi)->setCallback(callback, user_data);
}

void InputConnection::cancelCallback()
{
    dynamic_cast<RtMidiIn*>(rtmidi)->cancelCallback();
}

void InputConnection::ignoreTypes(bool sysex, bool timing, bool active_sensing)
{
    dynamic_cast<RtMidiIn*>(rtmidi)->ignoreTypes(sysex, timing, active_sensing);
}

OutputConnection::OutputConnection() : Connection()
{
}

void OutputConnection::initialize()
{
    rtmidi = new RtMidiOut();
}

void OutputConnection::finalize() noexcept
{
    if (rtmidi != nullptr)
    {
        close();
        delete dynamic_cast<RtMidiOut*>(rtmidi);
        rtmidi = nullptr;
    }
}

void OutputConnection::sendMessage(const ByteVec& message) const
{
    dynamic_cast<RtMidiOut*>(rtmidi)->sendMessage(&message);
}

} // Midi
} // StreichfettSse
