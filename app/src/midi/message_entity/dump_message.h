#pragma once

#include "midi/message_entity/message_entity.h"

namespace StreichfettSse
{
namespace Midi
{

class DumpMessage : public MessageEntity
{
public:
    explicit DumpMessage(const ByteVec& message, const std::string& name)
        : MessageEntity(message), is_validated_(false), name_(name)
    {
    }
    virtual ~DumpMessage() {}

    virtual void validate() = 0;

    DumpMessage() = delete;

protected:
    void fail(std::string_view cause_desc) const;
    void checkValidated() const;
    void setValidated() noexcept;
    void validateSysEx() const;

private:
    void failCauseNotValidated() const;
    bool is_validated_;
    std::string name_;
};

class DataDumpMessage : public DumpMessage
{
public:
    explicit DataDumpMessage(const ByteVec& message,
                             const std::string& name,
                             int data_index_first, int data_index_last)
        : DumpMessage(message, name),
        di_first_(data_index_first), di_last_(data_index_last)
    {
    }
    virtual ~DataDumpMessage() {}

    const ByteVec getDataBytes() const;

    DataDumpMessage() = delete;
    DataDumpMessage(const DataDumpMessage&) = delete;
    DataDumpMessage(DataDumpMessage&&) = delete;
    DataDumpMessage& operator=(const DataDumpMessage&) = delete;
    DataDumpMessage& operator=(DataDumpMessage&&) = delete;

protected:
    void validateDataSysEx() const;

private:
    int di_first_;
    int di_last_;
};

class DeviceInquiryDumpMessage final : public DumpMessage
{
public:
    explicit DeviceInquiryDumpMessage(const ByteVec& device_inquiry_dump)
        : DumpMessage(device_inquiry_dump, "Device Inquiry Dump")
    {
    }

    void validate() override;
    const DeviceInquiryResult getResult() const;

    DeviceInquiryDumpMessage() = delete;
    DeviceInquiryDumpMessage(const DeviceInquiryDumpMessage&) = delete;
    DeviceInquiryDumpMessage(DeviceInquiryDumpMessage&&) = delete;
    DeviceInquiryDumpMessage& operator=(const DeviceInquiryDumpMessage&) = delete;
    DeviceInquiryDumpMessage& operator=(DeviceInquiryDumpMessage&&) = delete;
};

class GlobalDumpMessage final : public DataDumpMessage
{
    static_assert(GLOBAL_DATA_INDEX_LAST < GLOBAL_DUMP_SIZE);

public:
    explicit GlobalDumpMessage(const ByteVec& global_dump)
        : DataDumpMessage(global_dump, "Global Dump",
                          GLOBAL_DATA_INDEX_FIRST, GLOBAL_DATA_INDEX_LAST)
    {
    }

    void validate() override;

    GlobalDumpMessage() = delete;
    GlobalDumpMessage(const GlobalDumpMessage&) = delete;
    GlobalDumpMessage(GlobalDumpMessage&&) = delete;
    GlobalDumpMessage& operator=(const GlobalDumpMessage&) = delete;
    GlobalDumpMessage& operator=(GlobalDumpMessage&&) = delete;
};

class SoundDumpMessage final : public DataDumpMessage
{
    static_assert(SOUND_DATA_INDEX_LAST < SOUND_DUMP_SIZE);

public:
    explicit SoundDumpMessage(const ByteVec& sound_dump)
        : DataDumpMessage(sound_dump, "Sound Dump",
                             SOUND_DATA_INDEX_FIRST, SOUND_DATA_INDEX_LAST)
    {
    }

    void validate() override;

    SoundDumpMessage() = delete;
    SoundDumpMessage(const SoundDumpMessage&) = delete;
    SoundDumpMessage(SoundDumpMessage&&) = delete;
    SoundDumpMessage& operator=(const SoundDumpMessage&) = delete;
    SoundDumpMessage& operator=(SoundDumpMessage&&) = delete;
};

} // Midi
} // StreichfettSse
