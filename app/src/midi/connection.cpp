#include "common.hpp"
#include "midi/connection.hpp"

namespace StreichfettSse
{
namespace Connector
{
Connection::Connection()
{
    resetPortInfo();
}

Connection::~Connection()
{
}

void Connection::open(const int port_index, const std::string& port_name)
{
    close();

    try
    {
        _port_index = port_index;
        _port_name = port_name;
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

unsigned int Connection::getPortCount()
{
    return rtmidi->getPortCount();
}

std::string Connection::getPortName(unsigned int port_index)
{
    return rtmidi->getPortName(port_index);
}

int Connection::getPortIndex() const noexcept
{
    return _port_index;
}

const std::string& Connection::getPortName() const noexcept
{
    return _port_name;
}

int Connection::getLastConnectedPortIndex() const noexcept
{
    return _last_connected_port_index;
}

int Connection::getLastFailedPortIndex() const noexcept
{
    return _last_failed_port_index;
}

void Connection::setLastConnectedPortIndex(const int port_index) noexcept
{
    _last_connected_port_index = port_index;
    _last_failed_port_index = -1;
}

void Connection::setLastFailedPortIndex(const int port_index) noexcept
{
    _last_failed_port_index = port_index;
    _last_connected_port_index = -1;
}

void Connection::resetPortInfo() noexcept
{
    _port_index = -1;
    _port_name = "";
    _last_connected_port_index = -1;
    _last_failed_port_index = -1;
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

void InputConnection::setCallback(RtMidiIn::RtMidiCallback callback, void* userData)
{
    dynamic_cast<RtMidiIn*>(rtmidi)->setCallback(callback, userData);
}

void InputConnection::cancelCallback()
{
    dynamic_cast<RtMidiIn*>(rtmidi)->cancelCallback();
}

void InputConnection::ignoreTypes(bool midiSysex, bool midiTime, bool midiSense)
{
    dynamic_cast<RtMidiIn*>(rtmidi)->ignoreTypes(midiSysex, midiTime, midiSense);
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

void OutputConnection::sendMessage(const ByteVec* message)
{
    dynamic_cast<RtMidiOut*>(rtmidi)->sendMessage(message);
}

} // Connector
} // StreichfettSse
