#include "tcpacceptor.h"

namespace bclasses {

TCPAcceptor::TCPAcceptorSPtr TCPAcceptor::createInstance(IO_service& service, char const* adress, unsigned short port)
{
    auto instance = Shared_ptr<TCPAcceptor>(new TCPAcceptor(service, adress, port));
    instance->doAccept();
    return instance;
}

TCPAcceptor::~TCPAcceptor()
{
    LOG_INFO_MESSAGE("Acceptor was destroyed");
    ErrorCode eCode;
    m_acceptor.cancel(eCode);
    if (eCode) {
        LOG_ERROR_MESSAGE(eCode.message());
    }
    m_acceptor.release(eCode);
    if (eCode) {
        LOG_ERROR_MESSAGE(eCode.message());
    }
    m_acceptor.close(eCode);
    if (eCode) {
        LOG_ERROR_MESSAGE(eCode.message());
    }
}

void TCPAcceptor::onAccept(TCPSession::TCPSessionSPtr const& session, ErrorCode const& error)
{
    TRACE_LOG;
    if (!error) {
        LOG_TRACE_MESSAGE(std::format("accept socket: {}", session->socket().remote_endpoint().port()));
        session->to_non_blocking_mode();
        session->execute();
        doAccept();
    } else {
        LOG_ERROR_MESSAGE(error.what());
    }
}

void TCPAcceptor::doAccept()
{
    TRACE_LOG;
    auto soc = TCPSession::createInstance(m_acceptor.get_executor());
    auto callback = [acceptor = std::move(shared_from_this()), soc = soc](ErrorCode const& eCode) {
        acceptor->onAccept(soc, eCode);
    };
    m_acceptor.async_accept(soc->socket(), std::move(callback));
}

TCPAcceptor::TCPAcceptor(IO_service& service, char const* adress, unsigned short port)
    : m_acceptor(service.get_executor())
{
    TRACE_LOG;
    auto tcpEndpoint = TCPEndpoint(IP::address::from_string(adress), port);
    try {
        m_acceptor.open(tcpEndpoint.protocol());
        m_acceptor.set_option(IP::tcp::acceptor::reuse_address(true));
        m_acceptor.set_option(IP::tcp::acceptor::enable_connection_aborted(true));
        m_acceptor.non_blocking(true);
        m_acceptor.bind(tcpEndpoint);
        m_acceptor.listen();
    } catch (std::exception& e) {
        LOG_ERROR_MESSAGE("TCPAcceptor Exception");
        LOG_ERROR_MESSAGE(e.what());
        ErrorCode er;
        m_acceptor.close(er);
        if (er) {
            LOG_ERROR_MESSAGE(er.message());
        }
        throw;
    }
}

} // namespace bclasses
