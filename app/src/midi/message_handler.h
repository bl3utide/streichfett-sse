﻿#pragma once

namespace StreichfettSse
{
namespace Midi
{

class MessageHandler
{
public:
    explicit MessageHandler(const ByteVec& message)
        : mbytes(message)
    {
    }
    virtual ~MessageHandler() {}

    MessageHandler() = delete;
    MessageHandler(const MessageHandler&) = delete;
    MessageHandler(MessageHandler&&) = delete;
    MessageHandler& operator=(const MessageHandler&) = delete;
    MessageHandler& operator=(MessageHandler&&) = delete;

protected:
    ByteVec mbytes;
};

enum class MidiMsg : int
{
    NoteOff,
    NoteOn,
    Other,
    _COUNT_,
};
class MidiMsgHandler : public MessageHandler
{
public:
    explicit MidiMsgHandler(const ByteVec& message)
        : MessageHandler(message)
    {
    }

    const MidiMsg getType() const noexcept;

    MidiMsgHandler() = delete;
    MidiMsgHandler(const MidiMsgHandler&) = delete;
    MidiMsgHandler(MidiMsgHandler&&) = delete;
    MidiMsgHandler& operator=(const MidiMsgHandler&) = delete;
    MidiMsgHandler& operator=(MidiMsgHandler&&) = delete;
};

class MidiMsgStringizer final : public MessageHandler
{
    explicit MidiMsgStringizer(const ByteVec& message)
        : MessageHandler(message)
    {
    }

    // TODO string format option?
    //  -> strinf formatってなんだっけ・・・
    const std::string toString() const noexcept;
    const std::string describe() const noexcept;

    MidiMsgStringizer() = delete;
    MidiMsgStringizer(const MidiMsgStringizer&) = delete;
    MidiMsgStringizer(MidiMsgStringizer&&) = delete;
    MidiMsgStringizer& operator=(const MidiMsgStringizer&) = delete;
    MidiMsgStringizer& operator=(MidiMsgStringizer&&) = delete;
};

class DumpHandler : public MessageHandler
{
public:
    explicit DumpHandler(const ByteVec& message, const std::string& name)
        : MessageHandler(message), is_validated_(false), name_(name)
    {
    }
    virtual ~DumpHandler() {}

    virtual void validate() = 0;

    DumpHandler() = delete;
    DumpHandler(const DumpHandler&) = delete;
    DumpHandler(DumpHandler&&) = delete;
    DumpHandler& operator=(const DumpHandler&) = delete;
    DumpHandler& operator=(DumpHandler&&) = delete;

protected:
    void fail(std::string_view cause_desc) const;
    void checkValidated() const;
    void setValidated() noexcept;
    void validateSysEx() const;

private:
    void failCauseNotValidated() const;
    bool isSysEx() const noexcept;
    bool is_validated_;
    std::string name_;
};

class DataDumpHandler : public DumpHandler
{
public:
    explicit DataDumpHandler(const ByteVec& message,
                             const std::string& name,
                             int data_index_first, int data_index_last)
        : DumpHandler(message, name),
        di_first_(data_index_first), di_last_(data_index_last)
    {
    }
    virtual ~DataDumpHandler() {}

    const ByteVec getDataBytes() const;

    DataDumpHandler() = delete;
    DataDumpHandler(const DataDumpHandler&) = delete;
    DataDumpHandler(DataDumpHandler&&) = delete;
    DataDumpHandler& operator=(const DataDumpHandler&) = delete;
    DataDumpHandler& operator=(DataDumpHandler&&) = delete;

protected:
    void validateDataSysEx() const;

private:
    int di_first_;
    int di_last_;
};

class DeviceInquiryDumpHandler final : public DumpHandler
{
public:
    explicit DeviceInquiryDumpHandler(const ByteVec& device_inquiry_dump)
        : DumpHandler(device_inquiry_dump, "Device Inquiry Dump")
    {
    }

    void validate() override;
    const DeviceInquiryResult getResult() const;

    DeviceInquiryDumpHandler() = delete;
    DeviceInquiryDumpHandler(const DeviceInquiryDumpHandler&) = delete;
    DeviceInquiryDumpHandler(DeviceInquiryDumpHandler&&) = delete;
    DeviceInquiryDumpHandler& operator=(const DeviceInquiryDumpHandler&) = delete;
    DeviceInquiryDumpHandler& operator=(DeviceInquiryDumpHandler&&) = delete;
};

class GlobalDumpHandler final : public DataDumpHandler
{
    static_assert(GLOBAL_DATA_INDEX_LAST < GLOBAL_DUMP_SIZE);

public:
    explicit GlobalDumpHandler(const ByteVec& global_dump)
        : DataDumpHandler(global_dump, "Global Dump",
                          GLOBAL_DATA_INDEX_FIRST, GLOBAL_DATA_INDEX_LAST)
    {
    }

    void validate() override;

    GlobalDumpHandler() = delete;
    GlobalDumpHandler(const GlobalDumpHandler&) = delete;
    GlobalDumpHandler(GlobalDumpHandler&&) = delete;
    GlobalDumpHandler& operator=(const GlobalDumpHandler&) = delete;
    GlobalDumpHandler& operator=(GlobalDumpHandler&&) = delete;
};


class SoundDumpHandler final : public DataDumpHandler
{
    static_assert(SOUND_DATA_INDEX_LAST < SOUND_DUMP_SIZE);

public:
    explicit SoundDumpHandler(const ByteVec& sound_dump)
        : DataDumpHandler(sound_dump, "Sound Dump",
                             SOUND_DATA_INDEX_FIRST, SOUND_DATA_INDEX_LAST)
    {
    }

    void validate() override;

    SoundDumpHandler() = delete;
    SoundDumpHandler(const SoundDumpHandler&) = delete;
    SoundDumpHandler(SoundDumpHandler&&) = delete;
    SoundDumpHandler& operator=(const SoundDumpHandler&) = delete;
    SoundDumpHandler& operator=(SoundDumpHandler&&) = delete;
};

} // Midi
} // StreichfettSse