#pragma once
#include <types.hpp>
#include <udpsocket.hpp>

class UDPReciver : public bclasses::EnableSharedFromThis<UDPReciver>
{
public:
    static bclasses::Shared_ptr<UDPReciver> instance(bclasses::CBDataFunc&& filterFunction,
                                                     bclasses::IO_service& service,
                                                     unsigned short port);
    bool onRead(bclasses::MessageStruct& msg, bclasses::ErrorCode const& code, size_t readSize);
    void setSocket(bclasses::UDPSocket::UDPSocketSPtr&& sock);
    bclasses::CBRFuntion dataReader();
    // #ifdef PERFOMANCE_TEST
    //std::future<bool> getFutureComlete() { return m_finished.get_future(); }
    // #endif
    unsigned getRecevedPackageCount() const noexcept { return m_packageCounter; };
// #ifdef PERFOMANCE_TEST
private:
    void updateCounters(uint8_t packageType, unsigned amount);
// # endif
private:
    UDPReciver(bclasses::CBDataFunc&& filterFunction);
    bclasses::CBDataFunc m_filterFunction;
    bclasses::UDPSocket::UDPSocketSPtr m_socket;
// #ifdef PERFOMANCE_TEST
   // std::promise<bool> m_finished;
    bclasses::AtomicUnsigned m_limitValue{0};
    bclasses::AtomicUnsigned m_packageCounter{0};
// #endif
};
