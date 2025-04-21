#include "json_data_reader.h"
#include "JsonTag.h"
#include "tracelog.h"
#include <boost/json/src.hpp>
#include <fstream>
#include <types.hpp>

namespace bclasses {
using JsonErrorCode = boost::json::error_code;
using boost::json::parse;
using JsonValue = boost::json::value;
using JsonObject = boost::json::object;
using JsonArray = boost::json::array;

using MessageVector = std::vector<MessageStruct>;

void fill_structure(MessageStruct& messageStruct, JsonObject& jsonObject)
{
    messageStruct.MessageSize = static_cast<uint16_t>(jsonObject.at(JsonTag::SizeTag).as_int64());
    messageStruct.MessageType = static_cast<uint8_t>(jsonObject.at(JsonTag::TypeTag).as_int64());
    messageStruct.MessageId = static_cast<uint64_t>(jsonObject.at(JsonTag::IdTag).as_int64());
    messageStruct.MessageData = static_cast<uint64_t>(jsonObject.at(JsonTag::DataTag).as_int64());
}

MessageVector data_parse(JsonValue& jValue, bool with_service_data)
{
    MessageVector lResult;
    try {
        auto& lJsonObject = jValue.as_object();
        auto& lJsonArray = lJsonObject.at(JsonTag::DataObjectName).as_array();
        LOG_INFO_MESSAGE("Json array size: {}", lJsonArray.size());
        auto&& dataSize = with_service_data ? lJsonArray.size() : lJsonArray.size() + 1;
        lResult.reserve(dataSize);
        if (with_service_data) {
            lResult.emplace_back(static_cast<uint16_t>(sizeof(MessageStruct)), ServicePackageType, dataSize, dataSize);
        }
        unsigned dataKeyCount{0};
        unsigned replacePackageIdCount{0};
        MessageStruct lMessage{};
        for (auto&& jRecord : lJsonArray) {
            fill_structure(lMessage, jRecord.get_object());
            if (with_service_data && lMessage.MessageType == ServicePackageType) {
                --lMessage.MessageType;
                ++replacePackageIdCount;
            }
            if (lMessage.MessageData == DataKey) {
                ++dataKeyCount;
            }
            lResult.emplace_back(lMessage);
        }
        LOG_INFO_MESSAGE("Vector size: {}\nReplace Packege Id Count: {}\nPackage with handling Id: {}\n",
                         lResult.size(),
                         replacePackageIdCount,
                         dataKeyCount);
        return lResult;

    } catch (std::exception& e) {
        LOG_ERROR_MESSAGE("Handled exception: ", e.what());
        lResult.clear();
    }
    return lResult;
}

MessageVector load_data_extend(std::string_view fileName, bool with_service_data)
{
    std::string lFileName(fileName);
    std::ifstream file(lFileName);
    if (!file.is_open()) {
        LOG_ERROR_MESSAGE("Can't open file: {}", lFileName);
        return {};
    }
    JsonErrorCode eCode;
    auto lJsonResult = parse(file, eCode);
    if (eCode) {
        LOG_ERROR_MESSAGE("Json loading error: {}", eCode.message());
        return {};
    }
    if (!lJsonResult.is_object()) {
        LOG_ERROR_MESSAGE("Unexpected object type, it hsould be \"object\"");
        return {};
    }
    return data_parse(lJsonResult, with_service_data);
}

MessageVector load_data_extend_debug(std::string_view fileName)
{
    return load_data_extend(fileName, true);
}
MessageVector load_data(std::string_view fileName)
{
    return load_data_extend(fileName, false);
}

} // namespace bclasses
