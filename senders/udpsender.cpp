#include "udpsender.hpp"
#include "sender_data1.hpp"
#include "sender_data2.hpp"
#include "sender_data3.hpp"
#include "sender_data4.hpp"

bclasses::Shared_ptr<UDPSender> UDPSender::instance(bclasses::IO_service& service, unsigned dataSet) {
  bclasses::Shared_ptr<UDPSender> producer;
  unsigned short port{0u};
  try {
    switch (dataSet) {
      case 1:
        producer.reset(new UDPSender(bclasses::IncomeData1));
        port = bclasses::UDPPortFirst;
        break;
      case 2:
        producer.reset(new UDPSender(bclasses::IncomeData2));
        port = bclasses::UDPPortFirst;
        break;
      case 3:
        producer.reset(new UDPSender(bclasses::IncomeData3));
        port = bclasses::UDPPortSecond;
        break;
      case 4:
        producer.reset(new UDPSender(bclasses::IncomeData4));
        port = bclasses::UDPPortSecond;
        break;
    }

    if ((producer != nullptr) && (port != 0u)) {
      auto sock = bclasses::UDPSocket::instanceClient(
        service, bclasses::DefaultAdress, port,
        std::bind(&UDPSender::doRead, producer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        std::bind(&UDPSender::doWrite, producer, std::placeholders::_1, std::placeholders::_2));
      producer->setSocket(std::move(sock));
      producer->startSendSequence();
    }
  } catch (const std::exception& e) {
    ERROR_LOG_MESSAGE(e.what());
  }
  return producer;
}

UDPSender::UDPSender(DArray& data)
  : m_data(data), m_begin(std::begin(data)), m_current(m_begin), m_end(std::end(m_data)) {}

bool UDPSender::doWrite(const bclasses::ErrorCode& error, size_t bTransferred) {
  if (!error) {
    if (bTransferred == sizeof(*m_current)) {
      ++m_current;
      if (m_current != m_end) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        m_sock->write(*m_current);
      } else {
        TRACE_LOG_MESSAGE("All data send");
        TRACE_LOG_MESSAGE(std::distance(m_begin,m_end));
        m_sock->close();
      }
    }
  } else {
    ERROR_LOG_MESSAGE(error.message());
  }
  return true;
}

bool UDPSender::doRead(bclasses::MessageStruct&& data, const bclasses::ErrorCode& error, size_t bTransferred) {
  TRACE_LOG_MESSAGE(error.message());
  return true;
}

void UDPSender::setSocket(bclasses::UDPSocket::UDPSocketSPtr&& sock) {
  m_sock = std::move(sock);
}

void UDPSender::startSendSequence() {
  m_sock->write(*m_current);
}
