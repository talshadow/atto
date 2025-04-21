#pragma once
#include <string_view>
namespace JsonTag {

using std::string_view;

constexpr string_view DataObjectName{"DataArray"};
constexpr string_view TypeTag{"type"};
constexpr string_view SizeTag{"size"};
constexpr string_view IdTag{"id"};
constexpr string_view DataTag{"data"};

} // namespace JsonTag
