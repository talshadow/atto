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
constexpr auto DefaultAdress = static_cast<char const*  >("127.0.0.1");
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

template <typename T>
using Decay = std::decay<T>;

using IO_service = boost::asio::io_service;
using TCPSock = boost::asio::ip::tcp::socket;
using TCPExecutor = boost::asio::ip::tcp::socket::executor_type;
using UDPSock = boost::asio::ip::udp::socket;
using TCPAccept = boost::asio::ip::tcp::acceptor;
using ErrorCode = boost::system::error_code;
using Worker = IO_service::work;
using WorkerPair = Pair<Thread, Unique_ptr<Worker> >;
using WorkerThreads = Vector<WorkerPair>;
using DataVector = Vector<uint8_t>;
using TCPEndpoint = boost::asio::ip::tcp::endpoint;
using UDPEndpoint = boost::asio::ip::udp::endpoint;
using String = std::string;
using CBFuntion = std::function<bool(const bclasses::ErrorCode&, size_t)>;
using CBRFuntion = std::function<bool(bclasses::MessageStruct&&, const bclasses::ErrorCode&, size_t)>;
using CBDataFunc = std::function<void(bclasses::MessageStruct&&) >;
using Unordered_map = std::unordered_map<decltype(MessageStruct::MessageId), MessageStruct>;
using Map = std::map<decltype(MessageStruct::MessageId), MessageStruct>;
using UnorderedMapPtr = Shared_ptr<Unordered_map>;



namespace IP = boost::asio::ip;
namespace ba = boost::asio;

struct TraceLog {
  TraceLog(const char* in) : str(in) { std::cout << str << " in \n"; }
  ~TraceLog() { std::cout << str << " out \n"; }

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
  long getDuration() { return (std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - m_value)).count(); }

 private:
  TimeValue m_value;
};

#define TRACE_LOG bclasses::TraceLog(static_cast<const char*>(__FUNCTION__))
#define TRACE_LOG_MESSAGE(msg)                    \
  do {                                            \
    bclasses::Cout << "TRACE: " << (msg) << "\n"; \
  } while (0)
#define WARNING_LOG_MESSAGE(msg)                    \
  do {                                              \
    bclasses::Cout << "WARNING: " << (msg) << "\n"; \
  } while (0)

#define ERROR_LOG_MESSAGE(msg)                    \
  do {                                            \
    bclasses::Cout << "ERROR: " << (msg) << "\n"; \
  } while (0)

}  // namespace bclasses
