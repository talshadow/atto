#include <tcpsocket.hpp>
#include <threadpool.hpp>

int main() {
  bclasses::TimeCalc timer;
    LOG_INFO_MESSAGE(std::format("data structure size: {}" , sizeof(bclasses::MessageStruct)));
  try {
      unsigned ThreadsCount = std::thread::hardware_concurrency() > 3
                                  ? 4
                                  : std::thread::hardware_concurrency() + 1;
      auto pool = bclasses::ThreadPool::createInstance(ThreadsCount);
      auto server = bclasses::TCPAcceptor::createInstance(pool->service(),
                                                          bclasses::DefaultAdress,
                                                          bclasses::TCPPort);
      while (std::cin.get() != 'e') {
      };

      LOG_TRACE_MESSAGE(std::format("Duration: {}", timer.getDuration()));
  } catch (std::exception &e) {
    LOG_ERROR_MESSAGE(e.what());
  }
  return 0;
}
