#include <future>
#include <iostream>
#include <messagesstore.hpp>
#include <threadpool.hpp>
#include <types.hpp>
#include "inputdata.hpp"

int main() {
  unsigned ThreadsCount = 3;
  auto pool = bclasses::ThreadPool::createInstance(ThreadsCount);
  bclasses::AHashMapStore store;
  bclasses::TimeCalc timer;
  std::promise<bool> p1;
  std::promise<bool> p2;

  auto f1 = p1.get_future();
  auto f2 = p2.get_future();

  (*pool)([&]() {

    for (const auto& record : DS::inputData1) {
      if (store.findSet(record)) {
        (*pool)([=]() { std::cout << record << '\n'; });
      }
    }
    p1.set_value(true);
  });

  (*pool)([&]() {
    std::cout << "\n\n";
    for (const auto& record : DS::inputData2) {
      if (store.findSet(record)) {
        (*pool)([=]() { std::cout << record << '\n'; });
      }
    }
    p2.set_value(true);
  });

  f1.get();
  f2.get();

  std::cout << "Time to output: " << std::dec << timer.getDuration() << " ms\n";
  std::cout << "Elements inserted: " << store.size() << '\n';
  bclasses::AUnsigned a_chack;
  std::cout << "Is lock free: " << a_chack.is_lock_free() << '\n';

  return 0;
}
