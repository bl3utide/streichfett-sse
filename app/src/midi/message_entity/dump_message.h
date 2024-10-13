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
    DumpMessage() = delete;
    virtual ~DumpMessage() = default;
    virtual void validate() = 0;

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
    DataDumpMessage() = delete;
    virtual ~DataDumpMessage() = default;
    const ByteVec getDataBytes() const;

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
    DeviceInquiryDumpMessage() = delete;
    void validate() override;
    const DeviceInquiryResult getResult() const;
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
    GlobalDumpMessage() = delete;
    void validate() override;
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
    SoundDumpMessage() = delete;
    void validate() override;
};

} // Midi
} // StreichfettSse
