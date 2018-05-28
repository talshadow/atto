#include "udpreciver.hpp"

UDPReciver::UDPReciver(bclasses::CBDataFunc&& filterFunction)
  : m_filterFunction{std::move(filterFunction)}, m_socket{nullptr} {
}

bclasses::Shared_ptr<UDPReciver> UDPReciver::instance(bclasses::CBDataFunc&& filterFunction,
                                                      bclasses::IO_service& service,
                                                      unsigned short port) {
  try {
    auto reciver = bclasses::Shared_ptr<UDPReciver>(new UDPReciver(std::move(filterFunction)));
    auto socket = bclasses::UDPSocket::instanceServer(service, port, reciver->dataReader());
    reciver->setSocket(std::move(socket));
    return reciver;
  } catch (std::exception& e) {
    ERROR_LOG_MESSAGE(e.what());
  }
  return bclasses::Shared_ptr<UDPReciver>();
}

bool UDPReciver::onRead(bclasses::MessageStruct&& msg, const bclasses::ErrorCode& code, size_t readSize) {
  if (!code) {
    if (sizeof(bclasses::MessageStruct) == readSize) {
      m_filterFunction(std::move(msg));
    } else {
      ERROR_LOG_MESSAGE(static_cast<const char*>("Wrong message size"));
    }
  } else {
    ERROR_LOG_MESSAGE(code.message());
    return false;
  }
  return true;
}

void UDPReciver::setSocket(bclasses::UDPSocket::UDPSocketSPtr&& sock) {
  m_socket = std::move(sock);
}

bclasses::CBRFuntion UDPReciver::dataReader() {
  return bclasses::CBRFuntion{std::bind(&UDPReciver::onRead, shared_from_this(), std::placeholders::_1,
                                        std::placeholders::_2, std::placeholders::_3)};
}
