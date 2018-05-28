#include <threadpool.hpp>
#include <types.hpp>
#include "tcpsender.hpp"
#include "udpreciver.hpp"

int main() {
  bclasses::TimeCalc timer;
  try {
    unsigned ThreadsCount = std::thread::hardware_concurrency() > 3 ? 4 : std::thread::hardware_concurrency() + 1;
    auto pool = bclasses::ThreadPool::createInstance(ThreadsCount);

    auto sender = TCPSender::instance(pool->service());

    auto reciver1 = UDPReciver::instance(sender->dataFunctor(), pool->service(), bclasses::UDPPortFirst);
    auto reciver2 = UDPReciver::instance(sender->dataFunctor(), pool->service(), bclasses::UDPPortSecond);

    std::cout << '\n' << timer.getDuration() << '\n';
    std::cin.get();
    std::cout << "Count of message: " << sender->count() << '\n';

  } catch (std::exception& e) {
    ERROR_LOG_MESSAGE(e.what());
  }

  return 0;
}
