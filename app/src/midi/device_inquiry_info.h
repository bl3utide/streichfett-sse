#pragma once

namespace StreichfettSse
{
namespace Midi
{

// DSI: Streichfett
class DeviceInquiryInfo final
{
public:
    explicit DeviceInquiryInfo(int device_id, const std::string_view ver)
        : received_(true), device_id_(device_id), firmware_version_(ver)
    {
    }
    explicit DeviceInquiryInfo()
        : received_(false), device_id_(-1), firmware_version_("")
    {
    }
    static const DeviceInquiryInfo makeFromMessage(const ByteVec& message);
    DeviceInquiryInfo& operator=(const DeviceInquiryInfo& rhs);
    DeviceInquiryInfo(const DeviceInquiryInfo& rhs) = delete;
    void initialize() noexcept;
    bool isReceived() const noexcept { return received_; }
    int getDeviceId() const noexcept { return device_id_; }
    const std::string getFirmwareVersion() const noexcept { return firmware_version_; }

private:
    bool received_;
    int device_id_;
    std::string firmware_version_;
};

} // namespace Midi
} // namespace StreichfettSse
