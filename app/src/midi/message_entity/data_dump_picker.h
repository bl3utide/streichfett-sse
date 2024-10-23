#pragma once

namespace StreichfettSse
{
namespace Midi
{

class DataDumpPicker
{
public:
    explicit DataDumpPicker(const ByteVec& message,
                            int data_index_first,
                            int data_index_last)
        : entity_(std::make_unique<MessageEntity>(message)),
        idx_first_(data_index_first),
        idx_last_(data_index_last)
    {
    }
    const ByteVec getDataBytes() const;

private:
    std::unique_ptr<MessageEntity> entity_;
    int idx_first_;
    int idx_last_;
};

class GlobalDumpPicker final : public DataDumpPicker
{
public:
    explicit GlobalDumpPicker(const ByteVec& message)
        : DataDumpPicker(message,
                         GLOBAL_DATA_INDEX_FIRST,
                         GLOBAL_DATA_INDEX_LAST)
    {
    }
};

class SoundDumpPicker final : public DataDumpPicker
{
public:
    explicit SoundDumpPicker(const ByteVec& message)
        : DataDumpPicker(message,
                         SOUND_DATA_INDEX_FIRST,
                         SOUND_DATA_INDEX_LAST)
    {
    }
};

} // Midi
} // StreichfettSse
