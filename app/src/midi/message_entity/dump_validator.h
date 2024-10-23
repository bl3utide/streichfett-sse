#pragma once

namespace StreichfettSse
{
namespace Midi
{

class MessageEntity;

class DumpValidator
{
public:
    explicit DumpValidator(const ByteVec& message, const std::string& name)
        : entity(std::make_unique<MessageEntity>(message)),
        is_valid_(false), name_(name)
    {
    }
    virtual ~DumpValidator() = default;
    virtual void validate() = 0;

protected:
    void fail(std::string_view cause_desc) const;
    void checkValidated() const;
    void setValidated() noexcept;
    void validateSysEx() const;
    std::unique_ptr<MessageEntity> entity;

private:
    bool is_valid_;
    std::string name_;
};

class DataDumpValidator : public DumpValidator
{
public:
    explicit DataDumpValidator(const ByteVec& message,
                               const std::string& name,
                               int data_index_first,
                               int data_index_last)
        : DumpValidator(message, name),
        di_first_(data_index_first), di_last_(data_index_last)
    {
    }
    virtual ~DataDumpValidator() = default;

protected:
    void validateDataSysEx() const;

private:
    int di_first_;
    int di_last_;
};

class DeviceInquiryDumpValidator final : public DumpValidator
{
public:
    explicit DeviceInquiryDumpValidator(const ByteVec& device_inquiry_dump)
        : DumpValidator(device_inquiry_dump, "Device Inquiry Dump")
    {
    }
    void validate() override;
};

class GlobalDumpValidator final : public DataDumpValidator
{
    static_assert(GLOBAL_DATA_INDEX_LAST < GLOBAL_DUMP_SIZE);

public:
    explicit GlobalDumpValidator(const ByteVec& global_dump)
        : DataDumpValidator(global_dump, "Global Dump",
                          GLOBAL_DATA_INDEX_FIRST, GLOBAL_DATA_INDEX_LAST)
    {
    }
    void validate() override;
};

class SoundDumpValidator final : public DataDumpValidator
{
    static_assert(SOUND_DATA_INDEX_LAST < SOUND_DUMP_SIZE);

public:
    explicit SoundDumpValidator(const ByteVec& sound_dump)
        : DataDumpValidator(sound_dump, "Sound Dump",
                             SOUND_DATA_INDEX_FIRST, SOUND_DATA_INDEX_LAST)
    {
    }
    void validate() override;
};

} // Midi
} // StreichfettSse
