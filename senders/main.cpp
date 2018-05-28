#include <future>
#include <iostream>

#include <threadpool.hpp>
#include "udpsender.hpp"



int main() {
  bclasses::TimeCalc timer;
  try{
    unsigned ThreadsCount = std::thread::hardware_concurrency() > 3 ? 4 : std::thread::hardware_concurrency() + 1;
    auto pool = bclasses::ThreadPool::createInstance(ThreadsCount);

    UDPSender::instance(pool->service(), 1);
    UDPSender::instance(pool->service(), 2);
    UDPSender::instance(pool->service(), 3);
    UDPSender::instance(pool->service(), 4);
    std::cout << '\n' << timer.getDuration() << '\n';
    std::cin.get();

  } catch( std::exception &e) {
    ERROR_LOG_MESSAGE(e.what());
  }


  return 0;
}
