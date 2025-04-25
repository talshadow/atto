#pragma once
#include "types.hpp"

namespace bclasses {
class UDPSocket : public EnableSharedFromThis<UDPSocket>
{
public:
    using UDPSocketSPtr = Shared_ptr<UDPSocket>;
    UDPSock& socket() { return m_socket; }
    static UDPSocketSPtr instanceServer(IO_service& service,
                                        unsigned short port,
                                        CBRFuntion&& lR = CBRFuntion(),
                                        CBFuntion&& lW = CBFuntion());
    static UDPSocketSPtr instanceClient(IO_service& service,
                                        std::string_view adress,
                                        unsigned short port,
                                        CBRFuntion&& lR = CBRFuntion(),
                                        CBFuntion&& lW = CBFuntion());

    void write(MessageStruct const& data);
    void execute();
    void onRead(ErrorCode const& error, size_t bTransferred);
    void onWrite(ErrorCode const& error, size_t bTransferred);
    void close();
    bool to_non_blocking_mode();

private:
    UDPSocket(IO_service& service, std::string_view ipAdress, unsigned short port, CBRFuntion lR, CBFuntion lW);
private:
    UDPSock m_socket;
    UDPEndpoint m_address;
    DataVector m_data;
    CBRFuntion logicR;
    CBFuntion logicW;
    MessageStruct sendData{};
};
} // namespace bclasses
