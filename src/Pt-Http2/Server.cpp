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
#include <Pt/Http2/Server.h>
#include <Pt/Http2/Request.h>
#include <Pt/Http2/Reply.h>
#include <Pt/Http2/Service.h>
#include <Pt/Http2/Responder.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Thread.h>
#include <Pt/System/IOStream.h>
#include <Pt/System/Timer.h>
#include <iostream>
#include <cassert>

namespace Pt {

namespace Http {

// TODO: rename class to Connection
class Handler : public Net::TcpSocket, public Connectable
{
    class ParseEvent : public HeaderParser::MessageHeaderEvent
    {
            Request& _request;

        public:
            explicit ParseEvent(Request& request)
            : HeaderParser::MessageHeaderEvent(request.header())
            , _request(request)
            { }

            virtual void onMethod(const std::string& method);
            virtual void onUrl(const std::string& url);
            virtual void onUrlParam(const std::string& q);
    };

    public:
        Handler(Server& server, Net::TcpServer& tcpServer);

        Handler(Server& server, System::EventLoop& loop, Net::TcpServer& tcpServer);

        ~Handler();

        void setLoop(System::EventLoop& loop);

        void onInput(System::StreamBuffer& sb);

        bool onOutput(System::StreamBuffer& sb);

        void onTimeout();

        bool doReply();

        void sendReply();

        bool isReady() const
        { return _parser.end() && _contentLength == 0; }

        const Request& request() const 
        { return _request; }

        const Reply& reply() const     
        { return _reply; }

        Signal<Handler&> inputReady;

        Signal<Handler&> timeout;

        System::StreamBuffer& buffer()         
        { return _stream.buffer(); }

    private:
        Server& _server;
        ParseEvent _parseEvent;
        HeaderParser _parser;
        Request _request;
        Reply _reply;

        System::Timer _timer;
        int _contentLength;
        Responder* _responder;
        System::IOStream _stream;
};


void Handler::ParseEvent::onMethod(const std::string& method)
{
    _request.method(method);
}


void Handler::ParseEvent::onUrl(const std::string& url)
{
    _request.url(url);
}


void Handler::ParseEvent::onUrlParam(const std::string& q)
{
    _request.qparams(q);
}


Handler::Handler(Server& server, Net::TcpServer& tcpServer)
: _server(server)
, _parseEvent(_request)
, _parser(_parseEvent, false)
, _responder(0)
{
    _stream.attachDevice(*this);
    Pt::connect(_stream.buffer().inputReady, *this, &Handler::onInput);
    Pt::connect(_stream.buffer().outputReady, *this, &Handler::onOutput);
    Pt::connect(_timer.timeout, *this, &Handler::onTimeout);

    Net::TcpSocket::accept(tcpServer, Net::TcpSocket::DEFER_ACCEPT);
}


void Handler::setLoop(System::EventLoop& loop)
{
    loop.add(*this);
    loop.add(_timer);

    //std::cerr << "beginRead" << std::endl;
    _stream.buffer().beginRead();
    _timer.start( _server.readTimeout() );
}


Handler::Handler(Server& server, System::EventLoop& loop, Net::TcpServer& tcpServer)
: _server(server)
, _parseEvent(_request)
, _parser(_parseEvent, false)
, _responder(0)
{
    loop.add(*this);
    loop.add(_timer);

    _stream.attachDevice(*this);
    Pt::connect(_stream.buffer().inputReady, *this, &Handler::onInput);
    Pt::connect(_stream.buffer().outputReady, *this, &Handler::onOutput);
    Pt::connect(_timer.timeout, *this, &Handler::onTimeout);

    Net::TcpSocket::accept(tcpServer, Net::TcpSocket::DEFER_ACCEPT);
    _stream.buffer().beginRead();
    _timer.start( _server.readTimeout() );
}


Handler::~Handler()
{
    if(_responder)
        _responder->release();
}


void Handler::onInput(System::StreamBuffer& sb)
{
    sb.endRead();

    if (sb.in_avail() == 0 || sb.device()->eof())
    {
        close();
        return;
    }

    _timer.start(_server.readTimeout());

    if ( _responder == 0 )
    {
        _parser.advance(sb);

        if (_parser.fail())
        {
            _responder = _server.getDefaultResponder(_request);
            _responder->replyError(_reply.body(), _request, _reply,
                std::runtime_error("invalid http header"));
            _responder->release();
            _responder = 0;

            sendReply();

            onOutput(sb);
            return;
        }

        if( _parser.end() )
        {
            _responder = _server.getResponder(_request);
            // TODO: test only_server.loop().commitEvent( AcceptEvent() );

            try
            {
                _responder->beginRequest(_stream, _request);
            }
            catch (const std::exception& e)
            {
                _reply.setHeader("Connection", "close");
                _responder->replyError(_reply.body(), _request, _reply, e);
                _responder->release();
                _responder = 0;
                sendReply();

                onOutput(sb);
                return;
            }

            _contentLength = _request.header().contentLength();

            //log_debug("content length of request is " << _contentLength);
            if (_contentLength == 0)
            {
                _timer.stop();
                doReply();
                return;
            }
        }
        else
        {
            sb.beginRead();
        }
    }

    if (_responder)
    {
        if (sb.in_avail() > 0)
        {
            try
            {
                std::size_t s = _responder->readBody(_stream);
                assert(s > 0);
                _contentLength -= s;
            }
            catch (const std::exception& e)
            {
                _reply.setHeader("Connection", "close");
                _responder->replyError(_reply.body(), _request, _reply, e);
                _responder->release();
                _responder = 0;
                sendReply();

                onOutput(sb);
                return;
            }
        }

        if (_contentLength <= 0)
        {
            _timer.stop();
            doReply();
        }
        else
        {
            sb.beginRead();
        }
    }
}


bool Handler::doReply()
{
    //log_trace("http::Socket::doReply");
    try
    {
        _responder->reply(_reply.body(), _request, _reply);
    }
    catch (const std::exception& e)
    {
        //log_warn("responder reported error: " << e.what());
        _reply.clear();
        _responder->replyError(_reply.body(), _request, _reply, e);
    }

    _responder->release();
    _responder = 0;

    sendReply();

    return onOutput(_stream.buffer());
}


bool Handler::onOutput(System::StreamBuffer& sb)
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
            bool keepAlive = _request.header().keepAlive()
                          && _reply.header().keepAlive();

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
                    _stream.buffer().beginRead();
            }
            else
            {
                //log_debug("don't do keep alive");
                close();
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


void Handler::onTimeout()
{
    //log_debug("timeout");
    timeout(*this);
}


void Handler::sendReply()
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
                << (_request.header().keepAlive() ? "keep-alive" : "close")
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


//////////////////////////////////////////////////////////////////////////
// Server
//////////////////////////////////////////////////////////////////////////

class ServerThread : public Connectable 
{
    public:
        ServerThread(Server& server)
        : _server(&server)
        , _thread(_loop)
        {
            _loop.event += Pt::slot(*this, &ServerThread::onAcceptEvent);
            _loop.event += Pt::slot(*this, &ServerThread::onExitEvent);
            _thread.start();
        }

        ServerThread::~ServerThread()
        {
            stop();
        }

        void serve(Handler* socket)
        {
            AcceptEvent ev(socket);
            _loop.commitEvent(ev);
        }

        void stop()
        {
            ServExitEvent ev;
            _loop.commitEvent(ev);

            _thread.join();
        }

    private:
        void onExitEvent(const ServExitEvent& ev)
        {
            std::vector<Handler*>::iterator it;
            for(it = _sockets.begin(); it != _sockets.end(); ++it)
            {
                delete *it;
            }

            _sockets.clear();
            _loop.exit();
        }

        void onAcceptEvent(const AcceptEvent& ev)
        {
            Handler* socket = ev.handler();

            _sockets.push_back(socket);
            socket->timeout += Pt::slot(*this, &ServerThread::onConnectionTimeout);

            socket->setLoop(_loop);
        }

        void onConnectionTimeout(Handler& handler)
        {
            //Socket* socket = event.socket();
            std::vector<Handler*>::iterator it;
            for(it = _sockets.begin(); it != _sockets.end(); ++it)
            {
                if(&handler == *it)
                {
                    delete *it;
                    _sockets.erase(it);
                    break;
                }
            }
        }

    private:
        Server* _server;
        Pt::System::MainLoop _loop;
        Pt::System::AttachedThread _thread;
        std::vector<Handler*> _sockets;
};


Server::Server(System::EventLoop& eventLoop)
: _loop(eventLoop)
, _useWorker(0)
{
    _defaultService = new NotFoundService();
    _noAuthService = new NotAuthenticatedService();

    _loop.add(_serverSocket);
    _serverSocket.connectionPending += Pt::slot(*this, &Server::onAccept);
}


Server::Server(System::EventLoop& eventLoop, const std::string& ip, unsigned short int port, int backlog)
: _loop(eventLoop)
, _serverSocket(ip, port, backlog)
, _useWorker(0)
{
    _defaultService = new NotFoundService();
    _noAuthService = new NotAuthenticatedService();

    this->startWorker();

    _loop.add(_serverSocket);
    _serverSocket.connectionPending += Pt::slot(*this, &Server::onAccept);
}


Server::~Server()
{
    std::vector<ServerThread*>::iterator threadIt;
    for(threadIt = _serverThreads.begin(); threadIt != _serverThreads.end(); ++threadIt)
    {
        (*threadIt)->stop();
        delete *threadIt;
    }

    std::vector<Handler*>::iterator it;
    for(it = _sockets.begin(); it != _sockets.end(); ++it)
    {
        delete *it;
    }

    delete _defaultService;
    delete _noAuthService;
}


void Server::listen(const std::string& ip, unsigned short int port, int backlog)
{
    this->startWorker();
    _serverSocket.listen(ip, port, backlog);
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
    Handler* socket = new Handler(*this, server);

    if(_useWorker < _serverThreads.size())
    {
        _serverThreads[_useWorker]->serve(socket);
        ++_useWorker;
    }
    else
    {
        socket->setLoop(_loop);
        _sockets.push_back(socket);
        socket->timeout += Pt::slot(*this, &Server::onConnectionTimeout);
        _useWorker = 0;
    }
}


void Server::onConnectionTimeout(Handler& handler)
{
    std::vector<Handler*>::iterator it;
    for(it = _sockets.begin(); it != _sockets.end(); ++it)
    {
        if(&handler == *it)
        {
            delete *it;
            _sockets.erase(it);
            break;
        }
    }
}


void Server::addService(const std::string& url, Service& service)
{
    System::WriteLock serviceLock(_serviceMutex);
    _services.insert(ServicesType::value_type(url, &service));
}


void Server::removeService(Service& service)
{
    System::WriteLock serviceLock(_serviceMutex);
    service.waitIdle();

    ServicesType::iterator it = _services.begin();
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


Responder* Server::getResponder(const Request& request)
{
    System::ReadLock serviceLock(_serviceMutex);

    for (ServicesType::const_iterator it = _services.lower_bound(request.url());
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


Responder* Server::getDefaultResponder(const Request& request)
{ 
    return _defaultService->createResponder(request); 
}


std::size_t Server::readTimeout() const
{
    return 20000;
}


std::size_t Server::writeTimeout() const
{
    return 20000;
}


std::size_t Server::keepAliveTimeout() const
{
    return 2000;
}


void Server::readTimeout(std::size_t ms)
{

}


void Server::writeTimeout(std::size_t ms)
{

}


void Server::keepAliveTimeout(std::size_t ms)
{

}


unsigned Server::minThreads() const
{
    return 1;
}


void Server::minThreads(unsigned m)
{
 
}


unsigned Server::maxThreads() const
{
    return 2;
}


void Server::maxThreads(unsigned m)
{

}

} // namespace Http

} // namespace Pt

