#include "common.hpp"
#include "midi/device_inquiry_info.h"

namespace StreichfettSse
{
namespace Midi
{

const DeviceInquiryInfo DeviceInquiryInfo::makeFromMessage(const ByteVec& message)
{
    const auto device_id = static_cast<int>(message.at(2));
    const auto firmware_version
        = std::format("{0}.{1}", message.at(10), message.at(11));
    return DeviceInquiryInfo(device_id, firmware_version);
}

DeviceInquiryInfo& DeviceInquiryInfo::operator=(const DeviceInquiryInfo& rhs)
{
    received_ = rhs.received_;
    device_id_ = rhs.device_id_;
    firmware_version_ = rhs.firmware_version_;
    return *this;
}

void DeviceInquiryInfo::initialize() noexcept
{
    received_ = false;
}

} // namespace Midi
} // namespace StreichfettSse
