#include "types.hpp"
#include <iomanip>

namespace bclasses {
std::ostream& operator<<(std::ostream& os, const MessageStruct& msg) {
  os << "{ 0x" << std::hex << msg.MessageSize << ", 0x" << std::hex << std::setw(2 * sizeof(msg.MessageType))
     << std::setfill('0') << static_cast<unsigned>(msg.MessageType) << ", 0x" << std::hex
     << std::setw(2 * sizeof(msg.MessageId)) << std::setfill('0') << msg.MessageId << ", 0x" << std::hex
     << std::setw(2 * sizeof(msg.MessageData)) << std::setfill('0') << msg.MessageData << " } ";
  return os;
}

bool operator==(const MessageStruct& lft, const MessageStruct& rgt) {
  return lft.MessageSize == rgt.MessageSize && lft.MessageType == rgt.MessageType && lft.MessageId == rgt.MessageId &&
         lft.MessageData == rgt.MessageData;
}

bool operator!=(const MessageStruct& lft, const MessageStruct& rgt) {
  return !(lft == rgt);
}

}  // namespace bclasses
