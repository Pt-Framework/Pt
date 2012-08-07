/*
 * Copyright (C) 2011 by Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Parser.h"
#include "NotFoundService.h"
#include "NotAuthenticatedService.h"
#include <Pt/Http/Server.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/Responder.h>

#ifdef PT_HTTP_WITH_SSL
#include <Pt/Ssl/IOBuffer.h>
#endif

#include <Pt/System/MainLoop.h>
#include <Pt/System/Thread.h>
#include <Pt/System/IOStream.h>
#include <Pt/System/IOBuffer.h>
#include <Pt/System/Timer.h>
#include <Pt/System/Logger.h>
#include <iostream>
#include <cassert>

log_define("Pt.Http.Server")

namespace Pt {

namespace Http {

class TcpConnection : public Http::Connection
                    , public Net::TcpSocket
                    , public Connectable
{
    class ParseEvent : public HeaderParser::MessageHeaderEvent
    {
            RequestHeader& _request;

        public:
            explicit ParseEvent(RequestHeader& request)
            : HeaderParser::MessageHeaderEvent(request)
            , _request(request)
            { }

            virtual void onMethod(const std::string& method);
            virtual void onUrl(const std::string& url);
            virtual void onUrlParam(const std::string& q);
    };

    public:
        TcpConnection(Server& server, Net::TcpServer& tcpServer);

        ~TcpConnection();

        void begin(System::EventLoop& loop);

        Signal<TcpConnection&> timeout;

    protected:
#ifdef PT_HTTP_WITH_SSL
        void onSslHandshake(Pt::Ssl::IOBuffer& ssl);

        void onSslInput(Pt::Ssl::IOBuffer& ssl);

        void onSslOutput(Pt::Ssl::IOBuffer& ssl);
#endif
        void onInput(System::StreamBuffer& sb);

        bool onOutput(System::StreamBuffer& sb);

        void onTimeout();

        void beginRead();

        void processInput();

        void endReply();

        void sendReply();

        bool isReady() const
        { return _parser.end() && _contentLength == 0; }

        const RequestHeader& request() const 
        { return _request; }

        const Reply& reply() const     
        { return _reply; }

    private:
        Server& _server;
        ParseEvent _parseEvent;
        HeaderParser _parser;
        RequestHeader _request;
        Reply _reply;
        System::EventLoop* _loop;
        System::Timer _timer;
        int _contentLength;
        Responder* _responder;
        System::IOBuffer _sockbuf;
        bool _ssl;
#ifdef PT_HTTP_WITH_SSL
        Ssl::IOBuffer _sslbuf;
#endif
        std::iostream _stream;
};


void TcpConnection::ParseEvent::onMethod(const std::string& method)
{
    _request.method(method);
}


void TcpConnection::ParseEvent::onUrl(const std::string& url)
{
    _request.url(url);
}


void TcpConnection::ParseEvent::onUrlParam(const std::string& q)
{
    _request.qparams(q);
}


TcpConnection::TcpConnection(Server& server, Net::TcpServer& tcpServer)
: _server(server)
, _parseEvent(_request)
, _parser(_parseEvent, false)
, _loop(0)
, _responder(0)
, _ssl(false)
#ifdef PT_HTTP_WITH_SSL
, _sslbuf( _sockbuf )
#endif
, _stream(0)
{
    Net::TcpSocket::accept(tcpServer);

#ifdef PT_HTTP_WITH_SSL
    _sslbuf.handshakeFinished() += slot(*this, &TcpConnection::onSslHandshake);
#endif
}


TcpConnection::~TcpConnection()
{
    if(_responder)
        _responder->release();
}


void TcpConnection::begin(System::EventLoop& loop)
{
    _reply.init(*this);
    _reply.clear();
    
    _timer.timeout() += Pt::slot(*this, &TcpConnection::onTimeout);
    _timer.setActive(loop);
    _timer.start( _server.readTimeout() );

    _loop = &loop;
    Http::Connection::init(loop);
    this->setActive(loop);
    
    _sockbuf.attach(*this);

#ifdef PT_HTTP_WITH_SSL
    // TODO: pass SSL context to TcpConnection::begin, which sets _ssl to true
    _ssl = false;

    if(_ssl)
    {
        log_debug("beginning HTTPS connection");
        _sslbuf.outputReady() += slot(*this, &TcpConnection::onSslOutput);
        _sslbuf.inputReady() += slot(*this, &TcpConnection::onSslInput);
        _stream.rdbuf(&_sockbuf);
        _sslbuf.beginAccept();
        return;
    }
#endif
        log_debug("beginning HTTP connection");
        _sockbuf.inputReady() += Pt::slot(*this, &TcpConnection::onInput);
        _sockbuf.outputReady() += Pt::slot(*this, &TcpConnection::onOutput);

        _stream.rdbuf(&_sockbuf);
        _sockbuf.beginRead();
}

#ifdef PT_HTTP_WITH_SSL
void TcpConnection::onSslHandshake(Pt::Ssl::IOBuffer& ssl)
{
    log_trace("TcpConnection::onAcceptHandshake");
    try 
    {
        ssl.endHandshake();
    }
    catch(...) 
    {
        log_error("accept handshake failed");
        return;
    }

    log_debug("peer name = " << ssl.peerName());
    log_debug("current cipher = " << ssl.currentCipher().name());
    ssl.beginRead();
}

void TcpConnection::onSslInput(Pt::Ssl::IOBuffer& ssl)
{
    log_trace("TcpConnection::onSslInput");

    try
    {
        ssl.endRead();
        processInput();
    }
    catch (const std::exception& e)
    {
        log_error("exception occured: " << e.what());
        return;
    }
}

void TcpConnection::onSslOutput(Pt::Ssl::IOBuffer& ssl)
{
}
#endif

void TcpConnection::onInput(System::StreamBuffer& sb)
{
    sb.endRead();
    processInput();
}


void TcpConnection::beginRead()
{
#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
        _sslbuf.beginRead();
    else
#endif
        _sockbuf.beginRead();
}


void TcpConnection::processInput()
{
    if (_stream.rdbuf()->in_avail() == 0 || Net::TcpSocket::eof())
    {
        close();
        timeout(*this);
        return;
    }

    _timer.start(_server.readTimeout());

    if ( _responder == 0 )
    {
        _parser.advance(_stream);

        if (_parser.fail())
        {
            _responder = _server.getDefaultResponder(_request);
            _responder->replyError(_reply.body(), _reply);
            _reply.finish();
            return;
        }

        if( _parser.end() )
        {
            _responder = _server.getResponder(_request);
            _responder->beginRequest(_stream, _request);
            _contentLength = _request.contentLength();

            //log_debug("content length of request is " << _contentLength);
            if (_contentLength == 0)
            {
                _timer.stop();
                _responder->beginReply(_reply.body(), _request, _reply);
                return;
            }
        }
        else
        {
            beginRead();
        }
    }

    if (_responder)
    {
        if (_stream.rdbuf()->in_avail() > 0)
        {
            std::size_t s = _responder->readBody(_stream, _reply);
            assert(s > 0);
            _contentLength -= s;

            if( _reply.finished() )
            {
                return;
            }
        }

        if (_contentLength <= 0)
        {
            _timer.stop();
            _responder->beginReply(_reply.body(), _request, _reply);
        }
        else
        {
            beginRead();
        }
    }
}


void TcpConnection::endReply()
{
    _responder->release();
    _responder = 0;
    sendReply();
    onOutput(_sockbuf);
}


bool TcpConnection::onOutput(System::StreamBuffer& sb)
{
    //log_trace("onOutput");

    //log_debug("send data to " << getPeerAddr());
    
    try
    {
        sb.endWrite();

        if ( sb.out_avail() )
        {
            sb.beginWrite();
            _timer.start(_server.writeTimeout());
        }
        else
        {
            bool keepAlive = _request.keepAlive() && _reply.header().keepAlive();

            if(keepAlive)
            {
                //log_debug("do keep alive");
                _timer.start(_server.keepAliveTimeout());
                _request.clear();
                _reply.clear();
                _parser.reset(false);

                if( sb.in_avail() )
                    onInput(sb);
                else
                    sb.beginRead();
            }
            else
            {
                //log_debug("don't do keep alive");
                close();
                timeout(*this); // TODO: notify server that socket is done
                return false;
            }
        }
    }
    catch (const std::exception& e)
    {
        //log_warn("exception occured when processing request: " << e.what());
        close();
        timeout(*this);
        return false;
    }

    return true;
}


void TcpConnection::onTimeout()
{
    //log_debug("timeout");
    timeout(*this);
}


void TcpConnection::sendReply()
{
    const char* contentLength = "Content-Length";
    const char* server = "Server";
    const char* connection = "Connection";
    const char* date = "Date";

    _stream << "HTTP/"
        << _reply.header().httpVersionMajor() << '.'
        << _reply.header().httpVersionMinor() << ' '
        << _reply.header().httpReturnCode() << ' '
        << _reply.header().httpReturnText() << "\r\n";

    for (ReplyHeader::const_iterator it = _reply.header().begin();
        it != _reply.header().end(); ++it)
    {
        _stream << it->first << ": " << it->second << "\r\n";
    }

    if (!_reply.header().hasHeader(contentLength))
    {
        _stream << "Content-Length: " << _reply.bodySize() << "\r\n";
    }

    if (!_reply.header().hasHeader(server))
    {
        _stream << "Server: Pt-Net-Server\r\n";
    }

    if (!_reply.header().hasHeader(connection))
    {
        _stream << "Connection: "
                << (_request.keepAlive() ? "keep-alive" : "close")
                << "\r\n";
    }

    if (!_reply.header().hasHeader(date))
    {
        char buffer[50];
        _stream << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
    }

    _stream << "\r\n";

    _reply.sendBody(_stream);
}


/*void TcpConnection::replyFinished()
{
    try
    {
        _responder->endReply(_reply.body(), _request, _reply);
    }
    catch (const std::exception& e)
    {
        _reply.clear();
        _responder->replyError(_reply.body(), _request, _reply, e);
    }
    
    _responder->release();
    _responder = 0;

    sendReply();

    onOutput(_stream.buffer());
}*/

//////////////////////////////////////////////////////////////////////////
// Server
//////////////////////////////////////////////////////////////////////////

class ServerThread : public Connectable 
{
    public:
        class ExitEvent : public Pt::BasicEvent<ExitEvent>
        {};
    
    
        class AcceptEvent : public Pt::BasicEvent<AcceptEvent>
        {
            public:
                AcceptEvent(TcpConnection* conn)
                : _conn(conn)
                { }
    
                TcpConnection* connection() const
                { return _conn; }
    
            private:
                TcpConnection* _conn;
        };

    public:
        ServerThread(Server& server)
        : _server(&server)
        , _thread(_loop)
        {
            _loop.event() += Pt::slot(*this, &ServerThread::onAcceptEvent);
            _loop.event() += Pt::slot(*this, &ServerThread::onExitEvent);
            _thread.start();
        }

        ~ServerThread()
        {
            stop();
        }

        void serve(TcpConnection* conn)
        {
            AcceptEvent ev(conn);
            _loop.commitEvent(ev);
        }

        void stop()
        {
            _loop.exit();
            _thread.join();

            std::vector<TcpConnection*>::iterator it;
            for(it = _connections.begin(); it != _connections.end(); ++it)
            {
                delete *it;
            }

            _connections.clear();
        }

    private:
        void onExitEvent(const ExitEvent& ev)
        {
        }

        void onAcceptEvent(const AcceptEvent& ev)
        {
            TcpConnection* conn = ev.connection();

            _connections.push_back(conn);
            conn->timeout += Pt::slot(*this, &ServerThread::onConnectionTimeout);

            conn->begin(_loop);
        }

        void onConnectionTimeout(TcpConnection& conn)
        {
            std::vector<TcpConnection*>::iterator it;
            for(it = _connections.begin(); it != _connections.end(); ++it)
            {
                if(&conn == *it)
                {
                    delete *it;
                    _connections.erase(it);
                    break;
                }
            }
        }

    private:
        Server* _server;
        Pt::System::MainLoop _loop;
        Pt::System::AttachedThread _thread;
        std::vector<TcpConnection*> _connections;
};


Server::Server(System::EventLoop& eventLoop)
: _loop(eventLoop)
, _useWorker(0)
, _maxThreads(1)
, _readTimeout(20000)
, _writeTimeout(20000)
, _keepAliveTimeout(30000)
{
    _defaultService = new NotFoundService();
    _noAuthService = new NotAuthenticatedService();

    _serverSocket.setActive(eventLoop);
    _serverSocket.connectionPending() += Pt::slot(*this, &Server::onAccept);
}


Server::Server(System::EventLoop& eventLoop, const std::string& ip, unsigned short int port, int backlog)
: _loop(eventLoop)
, _serverSocket(ip, port, backlog)
, _useWorker(0)
, _maxThreads(1)
, _readTimeout(20000)
, _writeTimeout(20000)
, _keepAliveTimeout(30000)
{
    _defaultService = new NotFoundService();
    _noAuthService = new NotAuthenticatedService();

    _serverSocket.setActive(_loop);
    _serverSocket.beginAccept();

    this->startWorker();
    
    _serverSocket.connectionPending() += Pt::slot(*this, &Server::onAccept);
}

Server::Server(System::EventLoop& eventLoop, const Pt::Net::AddrInfo& addr, int backlog)
: _loop(eventLoop)
, _serverSocket(addr, backlog)
, _useWorker(0)
, _maxThreads(1)
, _readTimeout(20000)
, _writeTimeout(20000)
, _keepAliveTimeout(30000)
{
    _defaultService = new NotFoundService();
    _noAuthService = new NotAuthenticatedService();

    _serverSocket.setActive(_loop);
    _serverSocket.beginAccept();

    this->startWorker();
    
    _serverSocket.connectionPending() += Pt::slot(*this, &Server::onAccept);
}

Server::~Server()
{
    std::vector<ServerThread*>::iterator threadIt;
    for(threadIt = _serverThreads.begin(); threadIt != _serverThreads.end(); ++threadIt)
    {
        (*threadIt)->stop();
        delete *threadIt;
    }

    std::vector<TcpConnection*>::iterator it;
    for(it = _connections.begin(); it != _connections.end(); ++it)
    {
        delete *it;
    }

    delete _defaultService;
    delete _noAuthService;
}


void Server::listen(const Pt::Net::AddrInfo& addr, int backlog)
{
    this->startWorker();
    _serverSocket.listen(addr, backlog);
    _serverSocket.beginAccept();
}


void Server::listen(const std::string& ip, unsigned short int port, int backlog)
{
    this->startWorker();
    _serverSocket.listen(ip, port, backlog);
    _serverSocket.beginAccept();
}


void Server::setHttps()
{
    // user must set a factory to create SSL context so we
    // can assign each worker thread its own thread-local
    // SSL context
}


void Server::startWorker()
{
    for(unsigned n = 1; n < this->maxThreads(); ++n)
    {
        ServerThread* st = new ServerThread(*this);
        _serverThreads.push_back(st);
    }

    _useWorker = _serverThreads.size();
}


void Server::onAccept(Net::TcpServer& server)
{
    // TODO: we should only pass the TcpSocket to the worker thread
    //       so that a Connection can be constructed with an event loop
    TcpConnection* conn = new TcpConnection(*this, server);

    if(_useWorker < _serverThreads.size())
    {
        _serverThreads[_useWorker]->serve(conn);
        ++_useWorker;
    }
    else
    {
        conn->begin(_loop);
        _connections.push_back(conn);
        conn->timeout += Pt::slot(*this, &Server::onConnectionTimeout);
        _useWorker = 0;
    }

    _serverSocket.beginAccept();
}


void Server::onConnectionTimeout(TcpConnection& conn)
{
    std::vector<TcpConnection*>::iterator it;
    for(it = _connections.begin(); it != _connections.end(); ++it)
    {
        if(&conn == *it)
        {
            delete *it;
            _connections.erase(it);
            break;
        }
    }
}


void Server::addService(const std::string& url, Service& service)
{
    System::WriteLock serviceLock(_serviceMutex);
    _services.insert(ServiceMap::value_type(url, &service));
}


void Server::removeService(Service& service)
{
    System::WriteLock serviceLock(_serviceMutex);
    service.waitIdle();

    ServiceMap::iterator it = _services.begin();
    while (it != _services.end())
    {
        if (it->second == &service)
        {
            _services.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}


Responder* Server::getResponder(const RequestHeader& request)
{
    System::ReadLock serviceLock(_serviceMutex);

    for (ServiceMap::const_iterator it = _services.lower_bound(request.url());
        it != _services.end() && it->first == request.url(); ++it)
    {
        if (!it->second->checkAuth(request))
        {
            return _noAuthService->createResponder(request, it->second->realm(), it->second->authContent());
        }

        Responder* resp = it->second->doCreateResponder(request);
        if (resp)
        {
            return resp;
        }
    }

    return _defaultService->createResponder(request);
}


Responder* Server::getDefaultResponder(const RequestHeader& request)
{ 
    return _defaultService->createResponder(request); 
}


std::size_t Server::readTimeout() const
{
    return _readTimeout;
}


void Server::setReadTimeout(std::size_t ms)
{
    _readTimeout = ms;
}


std::size_t Server::writeTimeout() const
{
    return _writeTimeout;
}


void Server::setWriteTimeout(std::size_t ms)
{
    _writeTimeout = ms;
}


std::size_t Server::keepAliveTimeout() const
{
    return _keepAliveTimeout;
}


void Server::setKeepAliveTimeout(std::size_t ms)
{
    _keepAliveTimeout = ms;
}


unsigned Server::maxThreads() const
{
    return _maxThreads;
}


void Server::setMaxThreads(unsigned m)
{
    _maxThreads = m;
}

} // namespace Http

} // namespace Pt

