#pragma once
#include "types.hpp"

namespace bclasses {
class UDPSocket : public Enable_shared_from_this<UDPSocket> {
 public:
  using UDPSocketSPtr = Shared_ptr<UDPSocket>;
  UDPSock& socket() { return m_socket; }
  static UDPSocketSPtr instanceServer(IO_service& service, unsigned short port,CBRFuntion&& lR = CBRFuntion(), CBFuntion&& lW= CBFuntion());
  static UDPSocketSPtr instanceClient(IO_service& service, const char* adress, unsigned short port,CBRFuntion&& lR = CBRFuntion(), CBFuntion&& lW= CBFuntion());

  void write(const MessageStruct& data);
  void execute();
  void onRead(const ErrorCode& error, size_t bTransferred);
  void onWrite(const ErrorCode& error, size_t bTransferred);
  void close();

 private:
  UDPSocket(IO_service& service, const char* adress, unsigned short port,CBRFuntion lR,CBFuntion lW);

  UDPSock m_socket;
  UDPEndpoint m_adress;
  DataVector m_data;
  CBRFuntion logicR;
  CBFuntion logicW;
  MessageStruct sendData;
};
}  // namespace bclasses
