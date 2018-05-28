#include "tcpsender.hpp"

bclasses::Shared_ptr<TCPSender> TCPSender::instance(bclasses::IO_service& service) {
  auto Inst = bclasses::Shared_ptr<TCPSender>(new TCPSender(service));
  return Inst;
}

bool TCPSender::onSend(const bclasses::ErrorCode& code, size_t size) {}

void TCPSender::write(bclasses::MessageStruct&& data) {
  uint8_t* pData = reinterpret_cast<uint8_t*>(&data);
  bclasses::DataVector sendData(pData, pData + sizeof(data));
  m_session->write(sendData);
}

void TCPSender::pushData(bclasses::MessageStruct&& data) {
  ++m_packageCounter;
  if (data.MessageData == bclasses::DataKey) {
    write(std::move(data));
  }
}

bclasses::CBDataFunc TCPSender::dataFunctor() {
  return bclasses::CBDataFunc{std::bind(&TCPSender::pushData, shared_from_this(), std::placeholders::_1)};
}

TCPSender::TCPSender(bclasses::IO_service& service)
  : m_packageCounter{0}
  , m_session{bclasses::TCPSession::createInstance(service, bclasses::DefaultAdress, bclasses::TCPPort)} {}
