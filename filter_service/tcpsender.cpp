#include "tcpsender.hpp"

bclasses::Shared_ptr<TCPSender> TCPSender::instance(const bclasses::TCPExecutor& tcp_executor) {
  auto Inst = bclasses::Shared_ptr<TCPSender>(new TCPSender(tcp_executor));
  return Inst;
}

bool TCPSender::onSend(const bclasses::ErrorCode &code, size_t size)
{
    return true;
}

void TCPSender::write(bclasses::MessageStruct&& data) {
  uint8_t* pData = reinterpret_cast<uint8_t*>(&data);
  bclasses::DataVector sendData(pData, pData + sizeof(data));
  m_session->write(sendData);
}

void TCPSender::pushData(bclasses::MessageStruct&& data) {
  ++m_packageCounter;
  if (m_store.findSet(data) && ( data.MessageData == bclasses::DataKey)) {
    write(std::move(data));
  }
}

bclasses::CBDataFunc TCPSender::dataFunctor() {
  return bclasses::CBDataFunc{std::bind(&TCPSender::pushData, shared_from_this(), std::placeholders::_1)};
}

TCPSender::TCPSender(const bclasses::TCPExecutor& tcp_executor)
  : m_packageCounter{0}
  , m_session{bclasses::TCPSession::createInstance(tcp_executor, bclasses::DefaultAdress, bclasses::TCPPort)} {}
