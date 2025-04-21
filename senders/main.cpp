#include <future>
#include "udpsender.hpp"
#include <threadpool.hpp>

constexpr auto SENDER_AMOUNT = 4U;

int main()
{
    /*std::atomic<int> atomicInt;
    std::atomic<unsigned> atomicUInt;
    std::atomic<int64_t> atomicInt64;
    std::atomic<uint64_t> atomicUInt64;

    unsigned iVal = 0b00001011;
    unsigned idop = ~iVal;
    unsigned iBitPos = (iVal + 1) & idop;

    std::cout << std::format("{:0>34B} {:0>34B} {:0>34B} {:>34}\n", iVal, idop, iBitPos,iBitPos);

    std::cout << std::format("std::atomic<int>: {} is lockfree {}\n",
                             sizeof(std::atomic<int>),
                             atomicInt.is_lock_free());
    std::cout << std::format("std::atomic<unsigned>: {} is lockfree {}\n",
                             sizeof(std::atomic<unsigned>),
                             atomicUInt.is_lock_free());
    std::cout << std::format("std::atomic<int64_t>: {} is lockfree {}\n",
                             sizeof(std::atomic<int64_t>),
                             atomicInt64.is_lock_free());
    std::cout << std::format("std::atomic<uint64_t>: {} is lockfree {}\n",
                             sizeof(std::atomic<uint64_t>),
                             atomicUInt64.is_lock_free());

    return 0;*/

    bclasses::TimeCalc timer;
    try {
        unsigned ThreadsCount = std::thread::hardware_concurrency() + 1;
        auto pool = bclasses::ThreadPool::createInstance(ThreadsCount);
        std::array<bclasses::Shared_ptr<UDPSender>, SENDER_AMOUNT> m_senders;
        LOG_INFO_MESSAGE(std::format("Duration after Thread Pool initialization: {}", timer.getDuration()));
        unsigned index = 0;
        for (auto& sender : m_senders) {
            sender = UDPSender::instance(pool->service(), (index % 4) + 1);
            ++index;
        }
        LOG_INFO_MESSAGE(std::format("Duration after starts: {}", timer.getDuration()));
        for (auto& sender : m_senders) {
            sender->getIsFinishedMarker().get();
        }
        LOG_INFO_MESSAGE(std::format("Duration: {}", timer.getDuration()));

    } catch (std::exception& e) {
        LOG_ERROR_MESSAGE(e.what());
    }
    return 0;
}
