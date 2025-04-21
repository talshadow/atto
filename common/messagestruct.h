#ifndef MESSAGESTRUCT_H
#define MESSAGESTRUCT_H
#include <cstdint>
#include <iosfwd>

namespace bclasses {

#pragma push pack(1)
struct MessageStruct
{
    uint16_t MessageSize;
    uint8_t MessageType;
    uint64_t MessageId;
    uint64_t MessageData;
};
#pragma pop

constexpr MessageStruct EndOfData{.MessageSize = sizeof(bclasses::MessageStruct),
                                  .MessageType = static_cast<decltype(MessageStruct::MessageType)>(-1),
                                  .MessageId = static_cast<decltype(MessageStruct::MessageId)>(-1),
                                  .MessageData = static_cast<decltype(MessageStruct::MessageData)>(-1)};

std::ostream& operator<<(std::ostream& ostream, MessageStruct const& msg);
bool operator==(MessageStruct const& lft, MessageStruct const& rgt);
bool operator!=(MessageStruct const& lft, MessageStruct const& rgt);
std::string to_string(MessageStruct const& messageStruct);

} // namespace bclasses

#endif // MESSAGESTRUCT_H
