#include <JsonTag.h>
#include <fstream>
#include <iostream>
#include <random>
#include <types.hpp>
#include <json_data_reader.h>
#include <boost/json.hpp>

bclasses::MessageStruct genMessage()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<decltype(bclasses::MessageStruct::MessageId)> id(0, 0x7FF);
    static std::uniform_int_distribution<decltype(bclasses::MessageStruct::MessageType)> type(0, 0x78);
    static std::uniform_int_distribution<decltype(bclasses::MessageStruct::MessageData)> data(0, 0xFF);
    return {sizeof(bclasses::MessageStruct), type(gen), id(gen), data(gen)};
}

int main()
{
    using JsonArray = boost::json::array;
    using JsonObject = boost::json::object;
    constexpr uint64_t kInterestingMessageId{10U};
    constexpr unsigned kLimit = ((1024 * 1024) / 24) + 1;
    JsonArray array;
    unsigned Data10{0};
    for (unsigned i = 0; i < kLimit; ++i) {
        bclasses::MessageStruct data{genMessage()};
        if (data.MessageType == bclasses::ServicePackageType) {
            data.MessageType = --data.MessageType;
        }
        JsonObject object;
        object.emplace(JsonTag::SizeTag, data.MessageSize);
        object.emplace(JsonTag::TypeTag, data.MessageType);
        object.emplace(JsonTag::IdTag, data.MessageId);
        object.emplace(JsonTag::DataTag, data.MessageData);
        array.emplace_back(std::move(object));
        if (data.MessageData == kInterestingMessageId) {
            ++Data10;
        }
    }
    JsonObject dataObjectJson{{JsonTag::DataObjectName, std::move(array)}};
    std::ofstream fileToSave("output_data.json");

    fileToSave << boost::json::serialize(dataObjectJson);
    fileToSave.close();
    std::cout << "Count of data 10: " << std::dec << Data10 << '\n';

    bclasses::load_data_extend_debug("output_data.json");
    return 0;
}
