#ifndef TRACELOG_H
#define TRACELOG_H

#include <cstdint>
#include <string_view>
#include <format>
#include <iostream>

constexpr void LOG_TRACE_MESSAGE(std::string_view pattern);
constexpr void LOG_INFO_MESSAGE(std::string_view pattern);
constexpr void LOG_WARNING_MESSAGE(std::string_view pattern);
constexpr void LOG_ERROR_MESSAGE(std::string_view pattern);

template<typename... T>
constexpr void LOG_TRACE_MESSAGE(std::string_view pattern, T&&... msg);

template<typename... T>
constexpr void LOG_WARNING_MESSAGE(std::string_view pattern, T&&... msg);

template<typename... T>
constexpr void LOG_ERROR_MESSAGE(std::string_view pattern, T&&... msg);

template<typename... T>
constexpr void LOG_INFO_MESSAGE(std::string_view pattern, T&&... msg);


namespace bclasses {

enum class LOG_MESSAGE_TYPE : std::uint8_t { eTrace, eInfo, eWarning, eError };

constexpr inline std::string_view LOG_MESSAGE_TYPE_TO_STRING(LOG_MESSAGE_TYPE type)
{
    switch (type) {
        case LOG_MESSAGE_TYPE::eTrace:
            return "TRACE: ";
        case LOG_MESSAGE_TYPE::eWarning:
            return "WARNING: ";
        case LOG_MESSAGE_TYPE::eError:
            return "ERROR: ";
        case LOG_MESSAGE_TYPE::eInfo:
            return "INFO: ";
    };
    return "UNDEFINED: ";
}

template<typename... Types>
constexpr void LOG_MESSAGE(LOG_MESSAGE_TYPE msgType, std::string_view fmtString, Types&&... msg)
{
    std::string realFmt;
    auto messageType = LOG_MESSAGE_TYPE_TO_STRING(msgType);
    realFmt.reserve(fmtString.size() + messageType.size() + 1);
    realFmt.append(messageType);
    realFmt.append(fmtString);
    realFmt.append("\n");
    std::cout << std::vformat(realFmt, std::make_format_args(msg...));
}

template<typename T>
constexpr void LOG_MESSAGE(LOG_MESSAGE_TYPE msgType, T&& msg)
{
    LOG_MESSAGE(msgType, "{}", std::forward<T>(msg));
}

struct TraceLog
{
    TraceLog(char const* input_param)
        : str(input_param)
    {
        LOG_TRACE_MESSAGE(" --> {} in", str);
    }
    ~TraceLog() { LOG_TRACE_MESSAGE(std::format(" <-- {} out", str)); }

    TraceLog(TraceLog&) = delete;
    TraceLog(TraceLog&&) = delete;
    TraceLog& operator=(TraceLog&) = delete;
    TraceLog& operator=(TraceLog&&) = delete;
    char const* str;
};

} // namespace bclasses

template<typename... T>
constexpr void LOG_TRACE_MESSAGE(std::string_view pattern, T&&... msg)
{
    LOG_MESSAGE(bclasses::LOG_MESSAGE_TYPE::eTrace, pattern, std::forward<T>(msg)...);
}

constexpr void LOG_TRACE_MESSAGE(std::string_view pattern)
{
    LOG_MESSAGE(bclasses::LOG_MESSAGE_TYPE::eTrace, pattern);
}

template<typename... T>
constexpr void LOG_WARNING_MESSAGE(std::string_view pattern, T&&... msg)
{
    LOG_MESSAGE(bclasses::LOG_MESSAGE_TYPE::eWarning, pattern, std::forward<T>(msg)...);
}

constexpr void LOG_WARNING_MESSAGE(std::string_view pattern)
{
    LOG_MESSAGE(bclasses::LOG_MESSAGE_TYPE::eWarning, pattern);
}


constexpr void LOG_ERROR_MESSAGE(std::string_view pattern)
{
    LOG_MESSAGE(bclasses::LOG_MESSAGE_TYPE::eError, pattern);
}

template<typename... T>
constexpr void LOG_ERROR_MESSAGE(std::string_view pattern, T&&... msg)
{
    LOG_MESSAGE(bclasses::LOG_MESSAGE_TYPE::eError, pattern, std::forward<T>(msg)...);
}

constexpr void LOG_INFO_MESSAGE(std::string_view pattern)
{
    LOG_MESSAGE(bclasses::LOG_MESSAGE_TYPE::eInfo, pattern);
}

template<typename... T>
constexpr void LOG_INFO_MESSAGE(std::string_view pattern, T&&... msg)
{
    LOG_MESSAGE(bclasses::LOG_MESSAGE_TYPE::eInfo, pattern, std::forward<T>(msg)...);
}

#ifdef __COUNTER__
#define TRACE_LOG bclasses::TraceLog TRACE##__LINE__##__COUNTER__(static_cast<const char*>(__FUNCTION__))
#elif
#define TRACE_LOG bclasses::TraceLog TRACE##__LINE__##__COUNTER__(static_cast<const char*>(__FUNCTION__))
#endif

#endif // TRACELOG_H
