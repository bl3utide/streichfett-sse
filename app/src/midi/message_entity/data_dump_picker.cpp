#include "common.hpp"
#include "midi/midi_common.hpp"
#include "midi/message_entity/message_entity.h"
#include "midi/message_entity/data_dump_picker.h"

namespace StreichfettSse
{
namespace Midi
{
const ByteVec DataDumpPicker::getDataBytes() const
{
    ByteVec data;

    for (auto i = idx_first_; i <= idx_last_; ++i)
    {
        data.push_back(entity_->at(i));
    }

    return data;
}

} // Midi
} // StreichfettSse
