#pragma once
#include <vector>
#include <string_view>
#include "messagestruct.h"

namespace bclasses {

std::vector<MessageStruct> load_data(std::string_view fileName);
std::vector<MessageStruct> load_data_extend_debug(std::string_view fileName);

}



