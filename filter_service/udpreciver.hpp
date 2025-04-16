#pragma once
#include <types.hpp>
#include <udpsocket.hpp>

class UDPReciver : public bclasses::Enable_shared_from_this<UDPReciver>
{
public:
    static bclasses::Shared_ptr<UDPReciver> instance(bclasses::CBDataFunc&& filterFunction,
                                                     bclasses::IO_service& service,
                                                     unsigned short port);
    bool onRead(bclasses::MessageStruct&& msg, bclasses::ErrorCode const& code, size_t readSize);
    void setSocket(bclasses::UDPSocket::UDPSocketSPtr&& sock);
    bclasses::CBRFuntion dataReader();

private:
    UDPReciver(bclasses::CBDataFunc&& filterFunction);
    bclasses::CBDataFunc m_filterFunction;
    bclasses::UDPSocket::UDPSocketSPtr m_socket;
};
