#pragma once
#include "types.hpp"
namespace bclasses {

class SharedMutex {
  static constexpr unsigned kCountOfTries = 1000;
  static constexpr unsigned kLockState = static_cast<unsigned>(-1);
  static constexpr unsigned kUnlockState = 0;

 public:
  SharedMutex() : m_read{kUnlockState}, m_writeState{kUnlockState},m_write{kUnlockState} {}
  ~SharedMutex() = default;

  SharedMutex(SharedMutex&) = delete;
  SharedMutex(SharedMutex&&) = delete;

  SharedMutex& operator=(SharedMutex&) = delete;
  SharedMutex& operator=(SharedMutex&&) = delete;

  void lock() noexcept {
    ++m_writeState;
    while (!tryLockWriter()){
      std::this_thread::yield();
    }
    while (!waitReaders()) {
      std::this_thread::yield();
    }
  }
  void unlock() noexcept {
    m_write.store(kUnlockState, std::memory_order_relaxed);
    --m_writeState;
  }

  void sharedLock() noexcept {
    while (!tryLockReader()) {
      std::this_thread::yield();
    }
  }

  void sharedUnlock() noexcept {
    --m_read;
  }

 private:
  bool waitReaders() noexcept {
    bool retVal = false;
    for (unsigned counter = 0; (counter < kCountOfTries) && (!retVal); ++counter) {
      retVal = (m_read.load() == kUnlockState);
    }
    return retVal;
  }

  bool tryLockWriter() noexcept {
    bool retVal = false;
    for (unsigned counter = 0; (counter < kCountOfTries) && (!retVal); ++counter) {
      unsigned lockState = kUnlockState;
      retVal = m_write.compare_exchange_weak(lockState,kLockState, std::memory_order_seq_cst) && (kUnlockState == lockState);
    }
    return retVal;
  }

  bool tryLockReader() noexcept {
    bool successLock{false};
    for (unsigned counter = 0; (counter < kCountOfTries) && (!(successLock = tryAddReader())); ++counter) {
    }
    return successLock;
  }

  bool tryAddReader() noexcept {
    if (m_writeState.load() == kUnlockState) {
      ++m_read;
      if (m_writeState.load() == kUnlockState) {
        return true;
      }
      --m_read;
    }
    return false;
  }

  AUnsigned m_read;
  AUnsigned m_writeState;
  AUnsigned m_write;
};

class ReaderMutex {
 public:
  ReaderMutex(SharedMutex& smutex) : m_mutex(smutex) {}
  ~ReaderMutex() = default;

  ReaderMutex(ReaderMutex&) = delete;
  ReaderMutex(ReaderMutex&&) = delete;

  ReaderMutex& operator=(ReaderMutex&) = delete;
  ReaderMutex& operator=(ReaderMutex&&) = delete;

  void lock() { m_mutex.sharedLock(); }
  void unlock() { m_mutex.sharedUnlock(); }

 private:
  SharedMutex& m_mutex;
};

}  // namespace bclasses
