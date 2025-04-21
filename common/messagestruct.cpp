#include "messagestruct.h"
#include <iomanip>
#include <format>
#include <ostream>


namespace bclasses {
std::ostream& operator<<(std::ostream& ostream, MessageStruct const& msg)
{
    ostream << "{ 0x" << std::hex << msg.MessageSize << ", 0x" << std::hex << std::setw(2 * sizeof(msg.MessageType))
    << std::setfill('0') << static_cast<unsigned>(msg.MessageType) << ", 0x" << std::hex
    << std::setw(2 * sizeof(msg.MessageId)) << std::setfill('0') << msg.MessageId << ", 0x" << std::hex
    << std::setw(2 * sizeof(msg.MessageData)) << std::setfill('0') << msg.MessageData << " } ";
    return ostream;
}

bool operator==(MessageStruct const& lft, MessageStruct const& rgt)
{
    return lft.MessageSize == rgt.MessageSize && lft.MessageType == rgt.MessageType && lft.MessageId == rgt.MessageId
           && lft.MessageData == rgt.MessageData;
}

bool operator!=(MessageStruct const& lft, MessageStruct const& rgt)
{
    return !(lft == rgt);
}

std::string to_string(MessageStruct const& messageStruct)
{
    return std::format("0x{:0>{}X}, 0x{:0>{}X}, 0x{:0>{}X}, 0x{:0>{}X}",
                    messageStruct.MessageSize,2*sizeof(messageStruct.MessageSize),
                    messageStruct.MessageType,2*sizeof(messageStruct.MessageType),
                    messageStruct.MessageId,2*sizeof(messageStruct.MessageId),
                    messageStruct.MessageData,2*sizeof(messageStruct.MessageData));
}
} // namespace bclasses

