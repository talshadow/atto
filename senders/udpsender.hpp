#pragma once
#include <types.hpp>
#include <udpsocket.hpp>
#include <future>

constexpr auto kDataSize = 100001;

using DArray = std::vector<bclasses::MessageStruct>;

class UDPSender : public bclasses::EnableSharedFromThis<UDPSender>
{
public:
    static bclasses::Shared_ptr<UDPSender> instance(bclasses::IO_service& service,
                                                    std::vector<bclasses::MessageStruct>&& DArray,
                                                    std::string_view address,
                                                    unsigned remotePort);
    std::future<bool> getIsFinishedMarker() { return m_isFinished.get_future(); }
    UDPSender(DArray&& data);
    ~UDPSender() = default;

private:
    bool doWrite(bclasses::ErrorCode const& error, size_t bTransferred);
    bool doRead(bclasses::MessageStruct& data, bclasses::ErrorCode const& error, size_t bTransferred);
    void setSocket(bclasses::UDPSocket::UDPSocketSPtr&& sock);

private:
    void startSendSequence();
    DArray m_data;
    DArray::iterator m_current;
    bclasses::UDPSocket::UDPSocketSPtr m_sock;
    std::promise<bool> m_isFinished;
};
