#pragma once
#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <map>
#include <optional>
#include <format>

template<typename T>
constexpr void LOG_TRACE_MESSAGE(T &&msg);

template<typename T>
constexpr void LOG_WARNING_MESSAGE(T &&msg);

template<typename T>
constexpr void LOG_ERROR_MESSAGE(T &&msg);

namespace bclasses {

#pragma push pack(1)
struct MessageStruct {
  uint16_t MessageSize;
  uint8_t MessageType;
  uint64_t MessageId;
  uint64_t MessageData;
};
#pragma pop

constexpr unsigned short UDPPortFirst = 0xA000;
constexpr unsigned short UDPPortSecond = 0xA001;
constexpr unsigned short TCPPort = 0xA00A;
constexpr auto DefaultAdress = "127.0.0.1";
constexpr uint64_t DataKey = 10;

constexpr MessageStruct EndOfData{
  sizeof(bclasses::MessageStruct), static_cast<decltype(MessageStruct::MessageType)>(-1),
  static_cast<decltype(MessageStruct::MessageId)>(-1), static_cast<decltype(MessageStruct::MessageData)>(-1)};


std::ostream& operator<<(std::ostream& os, const MessageStruct& msg);
bool operator==(const MessageStruct& lft, const MessageStruct& rgt);
bool operator!=(const MessageStruct& lft, const MessageStruct& rgt);

using Thread = std::thread;
using COutType = decltype(std::cout);
using ByteData = uint8_t;
using AUnsigned = std::atomic<unsigned>;

extern COutType& Cout;

template <typename T>
using Unique_ptr = std::unique_ptr<T>;

template <typename T>
using Shared_ptr = std::shared_ptr<T>;

template <typename T>
using UniqueLock = std::unique_lock<T>;

template <typename T>
using LockGuard = std::lock_guard<T>;

template <typename T, typename... Args>
Shared_ptr<T> make_shared(Args&&... arg) {
  return std::make_shared<std::remove_const<T>::type>(std::forward<Args>(arg)...);
}

template <typename T1, typename T2>
using Pair = std::pair<T1, T2>;

template <typename T>
using Vector = std::vector<T>;

template <typename T>
using Enable_shared_from_this = std::enable_shared_from_this<T>;

template<typename T>
using Optional = std::optional<T>;

template <typename T>
using Decay = std::decay<T>;

namespace ba = boost::asio;

using IO_service = ba::io_service;
using TCPSock = ba::ip::tcp::socket;
using TCPExecutor = ba::ip::tcp::socket::executor_type;
using UDPSock = ba::ip::udp::socket;
using TCPAccept = ba::ip::tcp::acceptor;
using ErrorCode = boost::system::error_code;
using Worker = ba::executor_work_guard<ba::io_context::executor_type>;
using WorkerPtr = std::unique_ptr<Worker>;
using WorkerPair = Pair<Optional<Thread>, WorkerPtr>;
using WorkerThreads = Vector<WorkerPair>;
using DataVector = Vector<uint8_t>;
using TCPEndpoint = ba::ip::tcp::endpoint;
using UDPEndpoint = ba::ip::udp::endpoint;
using String = std::string;
using CBFuntion = std::function<bool(const bclasses::ErrorCode&, size_t)>;
using CBRFuntion = std::function<bool(bclasses::MessageStruct&&, const bclasses::ErrorCode&, size_t)>;
using CBDataFunc = std::function<void(bclasses::MessageStruct&&) >;
using Unordered_map = std::unordered_map<decltype(MessageStruct::MessageId), MessageStruct>;
using Map = std::map<decltype(MessageStruct::MessageId), MessageStruct>;
using UnorderedMapPtr = Shared_ptr<Unordered_map>;

namespace IP = ba::ip;

enum class LOG_MESSAGE_TYPE : std::uint8_t { eTrace, eInfo, eWarning, eError };

constexpr inline const char *LOG_MESSAGE_TYPE_TO_STRING(LOG_MESSAGE_TYPE type)
{
    switch (type) {
    case LOG_MESSAGE_TYPE::eTrace:
        return "TRACE";
    case LOG_MESSAGE_TYPE::eWarning:
        return "WARNING";
    case LOG_MESSAGE_TYPE::eError:
        return "ERROR";
    case LOG_MESSAGE_TYPE::eInfo:
        return "INFO";
    };
    return "UNDEFINED";
}

template<typename T>
constexpr void LOG_MESSAGE(LOG_MESSAGE_TYPE msgType, T &&msg)
{
    bclasses::Cout << std::format("{}: {}\n",
                                  LOG_MESSAGE_TYPE_TO_STRING(msgType),
                                  std::forward<T>(msg));
}

struct TraceLog {
    TraceLog(const char* input_param) : str(input_param) { LOG_TRACE_MESSAGE(std::format(" --> {} in", str));}
    ~TraceLog() { LOG_TRACE_MESSAGE(std::format(" <-- {} out", str));}

    TraceLog(TraceLog&) = delete;
    TraceLog(TraceLog&&) = delete;
    TraceLog& operator=(TraceLog&) = delete;
    TraceLog& operator=(TraceLog&&) = delete;
    const char* str;
};

class TimeCalc {
public:
    using clock = std::chrono::high_resolution_clock;
    using TimeValue = decltype(clock::now());

    TimeCalc() : m_value{clock::now()} {}
    void reset() noexcept { m_value = clock::now(); }
    [[nodiscard]]long getDuration() const { return (std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - m_value)).count(); }

private:
    TimeValue m_value;
};

}  // namespace bclasses

template<typename T>
constexpr void LOG_TRACE_MESSAGE(T &&msg)
{
    LOG_MESSAGE(bclasses::LOG_MESSAGE_TYPE::eTrace, std::forward<T>(msg));
}

template<typename T>
constexpr void LOG_WARNING_MESSAGE(T &&msg)
{
    LOG_MESSAGE(bclasses::LOG_MESSAGE_TYPE::eWarning, std::forward<T>(msg));
}

template<typename T>
constexpr void LOG_ERROR_MESSAGE(T &&msg)
{
    LOG_MESSAGE(bclasses::LOG_MESSAGE_TYPE::eError, std::forward<T>(msg));
}

template<typename T>
constexpr void LOG_INFO_MESSAGE(T &&msg)
{
    LOG_MESSAGE(bclasses::LOG_MESSAGE_TYPE::eInfo, std::forward<T>(msg));
}

#define TRACE_LOG bclasses::TraceLog TRACE##__LINE__##__COUNTER__(static_cast<const char*>(__FUNCTION__))
