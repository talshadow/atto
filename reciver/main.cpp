#include <future>
#include <iostream>

#include <threadpool.hpp>
#include <tcpsocket.hpp>

int main() {
  bclasses::TimeCalc timer;
  try{
    unsigned ThreadsCount = std::thread::hardware_concurrency() > 3 ? 4 : std::thread::hardware_concurrency() + 1;
    auto pool = bclasses::ThreadPool::createInstance(ThreadsCount);
    auto server = bclasses::TCPAcceptor::createInstance(pool->service(),bclasses::DefaultAdress,bclasses::TCPPort);

    std::cout << '\n' << timer.getDuration() << '\n';
    std::cin.get();

  } catch( std::exception &e) {
    ERROR_LOG_MESSAGE(e.what());
  }


  return 0;
}
