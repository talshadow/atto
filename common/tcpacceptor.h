#pragma once
#include "types.hpp"
#include "tcpsocket.hpp"

namespace bclasses {

class TCPAcceptor : public EnableSharedFromThis<TCPAcceptor>
{
public:
    using TCPAcceptorSPtr = Shared_ptr<TCPAcceptor>;

    TCPAcceptor(TCPAcceptor&) = delete;
    TCPAcceptor(TCPAcceptor&&) = delete;
    TCPAcceptor& operator=(TCPAcceptor&) = delete;
    TCPAcceptor& operator=(TCPAcceptor&&) = delete;
    ~TCPAcceptor();

    static TCPAcceptorSPtr createInstance(IO_service& service, char const* adress, unsigned short port);
    void onAccept(TCPSession::TCPSessionSPtr const& session, ErrorCode const& error);

private:
    void doAccept();
    TCPAcceptor(IO_service& service, char const* adress, unsigned short port);

    TCPAccept m_acceptor;
};

} // namespace bclasses

