#include <future>
#include "udpsender.hpp"
#include <threadpool.hpp>

constexpr auto SENDER_AMOUNT = 16U;

int main()
{
    bclasses::TimeCalc timer;
    try {
        unsigned ThreadsCount = std::thread::hardware_concurrency() + 1;
        auto pool = bclasses::ThreadPool::createInstance(ThreadsCount);
        std::array<bclasses::Shared_ptr<UDPSender>, SENDER_AMOUNT> m_senders;
        LOG_INFO_MESSAGE(std::format("Duration after Thread Pool initialization: {}", timer.getDuration()));
        unsigned index = 0;
        for (auto& sender : m_senders) {
            sender = UDPSender::instance(pool->service(), index%4+1);
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
