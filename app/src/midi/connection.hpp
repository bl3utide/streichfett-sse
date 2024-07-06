#pragma once

namespace StreichfettSse
{
namespace Connector
{

class Connection
{
public:
    Connection();
    virtual ~Connection();
    virtual void initialize() = 0;
    virtual void finalize() noexcept = 0;
    void open(int port_index, const std::string& port_name);
    virtual void close() noexcept;
    bool isPortOpen() const;
    unsigned int getPortCount() const;
    const std::string getPortName(unsigned int port_index) const;
    int getPortIndex() const noexcept;
    const std::string& getPortName() const noexcept;
    int getLastConnectedPortIndex() const noexcept;
    int getLastFailedPortIndex() const noexcept;

protected:
    void setLastConnectedPortIndex(int port_index) noexcept;
    void setLastFailedPortIndex(int port_index) noexcept;
    RtMidi* rtmidi;

private:
    int port_index_;
    std::string port_name_;
    int last_connected_port_index_;
    int last_failed_port_index_;
    void resetPortInfo() noexcept;
};

class InputConnection : public Connection
{
public:
    InputConnection();
    void initialize() override;
    void finalize() noexcept override;
    void close() noexcept override;
    void setCallback(RtMidiIn::RtMidiCallback callback, void* userData = (void*)0);
    void cancelCallback();
    void ignoreTypes(bool midiSysex, bool midiTime, bool midiSense);
};

class OutputConnection : public Connection
{
public:
    OutputConnection();
    void initialize() override;
    void finalize() noexcept override;
    void sendMessage(const ByteVec* message);
};

} // Connector
} // StreichfettSse
