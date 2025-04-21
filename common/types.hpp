#pragma once
#include "messagestruct.h"
#include "timecalc.h"
#include "tracelog.h"

#include <atomic>
#include <boost/asio.hpp>
#include <cstdint>
#include <format>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace bclasses {

constexpr unsigned short UDPPortFirst = 0xA000;
constexpr unsigned short UDPPortSecond = 0xA001;
constexpr unsigned short TCPPort = 0xA00A;
constexpr auto DefaultAdress = "127.0.0.1";
constexpr uint64_t DataKey = 10;
inline constexpr uint8_t ServicePackageType{0xFF}; // It is used for debugging purposes

using Thread = std::thread;
using ByteData = uint8_t;
using AtomicUnsigned = std::atomic<unsigned>;

template<typename T>
using Unique_ptr = std::unique_ptr<T>;

template<typename T>
using Shared_ptr = std::shared_ptr<T>;

template<typename T>
using UniqueLock = std::unique_lock<T>;

template<typename T>
using LockGuard = std::lock_guard<T>;

template<typename T, typename... Args>
Shared_ptr<T> make_shared(Args&&... arg)
{
    return std::make_shared<std::remove_const<T>::type>(std::forward<Args>(arg)...);
}

template<typename T1, typename T2>
using Pair = std::pair<T1, T2>;

template<typename T>
using Vector = std::vector<T>;

template<typename T>
using Enable_shared_from_this = std::enable_shared_from_this<T>;

template<typename T>
using Optional = std::optional<T>;

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
using CBFuntion = std::function<bool(bclasses::ErrorCode const&, size_t)>;
using CBRFuntion = std::function<bool(bclasses::MessageStruct&&, bclasses::ErrorCode const&, size_t)>;
using CBDataFunc = std::function<void(bclasses::MessageStruct&&)>;
using Unordered_map = std::unordered_map<decltype(MessageStruct::MessageId), MessageStruct>;
using Map = std::map<decltype(MessageStruct::MessageId), MessageStruct>;
using UnorderedMapPtr = Shared_ptr<Unordered_map>;

namespace IP = ba::ip;
} // namespace bclasses


