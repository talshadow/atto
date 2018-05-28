#include "udpsocket.hpp"
namespace bclasses {

UDPSocket::UDPSocketSPtr UDPSocket::instanceServer(IO_service& service,
                                                   unsigned short port,
                                                   CBRFuntion&& lR,
                                                   CBFuntion&& lW) {
  try {
    auto socket = UDPSocket::UDPSocketSPtr(new UDPSocket(service, nullptr, port, std::move(lR), std::move(lW)));
    socket->execute();
    return socket;
  } catch (std::exception& e) {
    ERROR_LOG_MESSAGE(e.what());
  }
  return UDPSocket::UDPSocketSPtr();
}

UDPSocket::UDPSocketSPtr UDPSocket::instanceClient(IO_service& service,
                                                   const char* adress,
                                                   unsigned short port,
                                                   CBRFuntion&& lR,
                                                   CBFuntion&& lW) {
  auto socket = UDPSocket::UDPSocketSPtr(new UDPSocket(service, adress, port, std::move(lR), std::move(lW)));
  socket->execute();
  return socket;
}

void UDPSocket::write(const MessageStruct& data) {
  sendData = data;
  m_socket.async_send_to(
    ba::buffer(&sendData, sizeof(sendData)), m_adress,
    std::bind(&UDPSocket::onWrite, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void UDPSocket::execute() {
  m_socket.async_receive_from(
    ba::buffer(m_data.data(), m_data.size()), m_adress,
    std::bind(&UDPSocket::onRead, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void UDPSocket::onRead(const ErrorCode& error, size_t bTransferred) {
  MessageStruct* pData = reinterpret_cast<MessageStruct*>(m_data.data());
  MessageStruct data{*pData};

  if ((logicR && logicR(std::move(data), error, bTransferred)) || (!logicR && !error)) {
    execute();
  } else {
    close();
  }
}

void UDPSocket::onWrite(const ErrorCode& error, size_t bTransferred) {
  if ((logicR && !logicW(error, bTransferred)) || (!logicR && error)) {
    close();
  }
}

void UDPSocket::close() {}

UDPSocket::UDPSocket(IO_service& service, const char* adress, unsigned short port, CBRFuntion lR, CBFuntion lW)
  : m_socket(service, UDPEndpoint(IP::udp::v4(), adress ? 0 : port))
  , m_adress(nullptr == adress ? UDPEndpoint() : UDPEndpoint(IP::address::from_string(adress), port))
  , m_data(sizeof(MessageStruct))
  , logicR{std::move(lR)}
  , logicW{std::move(lW)} {
  std::stringstream strm;
  strm << "local port : " << std::hex << m_socket.local_endpoint().port() << "\nis_open: " << m_socket.is_open();
  strm << "\nlocal port : " << std::hex << m_adress.port() << "\nadress: " << m_adress.address().to_string();
  TRACE_LOG_MESSAGE(strm.str());
}

}  // namespace bclasses
