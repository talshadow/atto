#pragma once
#include <messagesstore.hpp>
#include <tcpsocket.hpp>
#include <types.hpp>

class TCPSender : public bclasses::Enable_shared_from_this<TCPSender>
{
public:
    static bclasses::Shared_ptr<TCPSender> instance(bclasses::TCPExecutor const& tcp_executor);

    bool onSend(bclasses::ErrorCode const&, size_t);
    void write(bclasses::MessageStruct&& data);
    void pushData(bclasses::MessageStruct&& data);
    unsigned long count() const noexcept { return m_packageCounter.load(std::memory_order_relaxed); }

    bclasses::CBDataFunc dataFunctor();

private:
    TCPSender(bclasses::TCPExecutor const& tcp_executor);

private:
    bclasses::AUnsigned m_packageCounter;
    bclasses::TCPSession::TCPSessionSPtr m_session;
    bclasses::MessageStruct m_msg;
    bclasses::AHashMapStore m_store;
};
