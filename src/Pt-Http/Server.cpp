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

#include "Connection.h"
#include "NotFoundService.h"
#include "NotAuthenticatedService.h"
#include <Pt/Http/Server.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/Responder.h>

#ifdef PT_HTTP_WITH_SSL
#include <Pt/Ssl/Context.h>
#endif

#include <Pt/System/MainLoop.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Logger.h>
#include <memory>
#include <cassert>

log_define("Pt.Http.Server")

namespace Pt {

namespace Http {

class RequestHandler : public Pt::Connectable
{
    public:
        RequestHandler(Server& server, Net::TcpServer& tcpServer);

        ~RequestHandler();

        void setSecure(Ssl::Context& ctx)
        {
            _conn.setSecure(ctx);
        }

        void beginServe(System::EventLoop& loop);

        Signal<RequestHandler&>& timeout()
        { return _timeout; }

        Service* service()
        { return _service; }

    protected:
        void onTimeout(Connection&)
        { _timeout.send(*this); }

        void onRequestReceived(Request& req);

        void onReplySent(Reply& r);

        void replyError();

    private:
        Server& _server;
        Service* _service;
        Responder* _responder;
        Connection _conn;
        Request _request;
        Reply _reply;
        bool _ignoreBody;
        Signal<RequestHandler&> _timeout;
};


RequestHandler::RequestHandler(Server& server, Net::TcpServer& tcpServer)
: _server(server)
, _service(0)
, _responder(0)
, _conn()
, _ignoreBody(false)
{
    _conn.accept(tcpServer);
    _request.inputReceived() += Pt::slot(*this, &RequestHandler::onRequestReceived);
    _reply.outputSent() += Pt::slot(*this, &RequestHandler::onReplySent);
}


RequestHandler::~RequestHandler()
{
    if(_responder)
    {
        _responder->release();
    }
}


void RequestHandler::beginServe(System::EventLoop& loop)
{  
    log_trace("RequestHandler::beginServe");

    _ignoreBody = false;

    _conn.setActive(loop);

    _reply.init(_conn);
    _reply.clear();

    _request.init(_conn);
    _request.clear();
    _request.beginReceive();
}


void RequestHandler::onRequestReceived(Request& req)
{
    log_trace("RequestHandler::onRequestReceived");

    // TODO: error reply on HTTP exception
    //_responder = _server.getDefaultResponder(_request->header());
    //replyError();

    MessageProgress progress = _request.endReceive();

    if( ! _conn.isConnected() )
    {
        log_debug("not connected anymore");
        _timeout.send(*this);
        return;
    }

    if( progress.header() )
    {
        log_debug("received request header");
        _responder = _server.getResponder(_request.header());
        _responder->beginRequest( _request );
        _ignoreBody = false;
    }
    
    if( progress.body() )
    {
        std::streambuf* sb = _request.body().rdbuf();
        std::streamsize avail = sb->in_avail();

        log_debug("body available: " << avail );
        
        if(_ignoreBody)
        {
            while(avail--)
                sb->sbumpc();
        }
        else
        {
            _responder->readRequest(_request, _reply);

            // TODO: _reply.isSending(), maybe only beginSend was called
            //       for a chunked reply.
            if( _reply.finished() )
            {
                _ignoreBody = true;
                return;
            }
        }
    }

    if( progress.finished() )
    {
        if(_responder)
        {
            log_debug("request body finished, begin reply");
            _responder->beginReply(_request, _reply);
            return;
        }
    }

    log_debug("more data available");
    _request.beginReceive();
}


void RequestHandler::onReplySent(Reply& r)
{
    log_trace("RequestHandler::onReplySent");

    MessageProgress progress = _reply.endSend();

    if( ! _conn.isConnected() )
    {
        log_debug("not connected anymore");
        _timeout.send(*this);
        return;
    }

    if( ! progress.finished() )
    {
        log_debug("writing left over data");
        _reply.beginSend();
        return;
    }

    if( ! _reply.finished() )
    {
        log_debug("continuing response");
        _reply.clearBody();
        _responder->writeReply(_request, _reply);
        return;
    }

    log_debug("response finished");

    if( _responder )
    {
        _responder->release();
        _responder = 0;
    }

    _reply.clear();
    _request.clear();

    _request.beginReceive();
}


void RequestHandler::replyError()
{
    _reply.clear();

    _reply.header().httpReturn(500, "internal server error");
    _reply.header().setHeader("Content-Type", "text/plain");
    _reply.header().setHeader("Connection", "close");
    _reply.body() << "Error 500: Internal server error.";

    _reply.finish();
    _reply.beginSend();
}


class ServerThread : public Connectable 
{
    public:
        class ExitEvent : public Pt::BasicEvent<ExitEvent>
        {};
    
    
        class AcceptEvent : public Pt::BasicEvent<AcceptEvent>
        {
            public:
                AcceptEvent(RequestHandler* conn)
                : _conn(conn)
                { }
    
                RequestHandler* connection() const
                { return _conn; }
    
            private:
                RequestHandler* _conn;
        };

    public:
        ServerThread(Server& server, Ssl::Context* sslctx = 0)
        : _server(&server)
        , _ssl(false)
        , _thread(_loop)
        {
#ifdef PT_HTTP_WITH_SSL
            if(sslctx)
            {
                _sslctx.assign(*sslctx);
                _ssl = true;
            }
#endif

            _loop.event() += Pt::slot(*this, &ServerThread::onAcceptEvent);
            _loop.event() += Pt::slot(*this, &ServerThread::onExitEvent);
            _thread.start();
        }

        ~ServerThread()
        {
            stop();
        }

        void serve(RequestHandler* conn)
        {
            AcceptEvent ev(conn);
            _loop.commitEvent(ev);
        }

        void stop()
        {
            _loop.exit();
            _thread.join();

            std::vector<RequestHandler*>::iterator it;
            for(it = _handler.begin(); it != _handler.end(); ++it)
            {
                delete *it;
            }

            _handler.clear();
        }

    private:
        void onExitEvent(const ExitEvent& ev)
        {
        }

        void onAcceptEvent(const AcceptEvent& ev)
        {
            RequestHandler* handler = ev.connection();

            _handler.push_back(handler);
            handler->timeout() += Pt::slot(*this, &ServerThread::onConnectionTimeout);

#ifdef PT_HTTP_WITH_SSL
            if(_ssl)
                handler->setSecure(_sslctx);
#endif

            handler->beginServe(_loop);
        }

        void onConnectionTimeout(RequestHandler& conn)
        {
            std::vector<RequestHandler*>::iterator it;
            for(it = _handler.begin(); it != _handler.end(); ++it)
            {
                if(&conn == *it)
                {
                    delete *it;
                    _handler.erase(it);
                    break;
                }
            }
        }

    private:
        Server* _server;
        Pt::System::MainLoop _loop;
        bool _ssl;
#ifdef PT_HTTP_WITH_SSL
        Ssl::Context _sslctx;
#endif
        Pt::System::AttachedThread _thread;
        std::vector<RequestHandler*> _handler;
};


Server::Server()
: _loop(0)
, _sslctx(0)
, _ssl(false)
, _useWorker(0)
, _maxThreads(1)
, _readTimeout(20000)
, _writeTimeout(20000)
, _keepAliveTimeout(30000)
{
    _notFoundService = new NotFoundService();
    _noAuthService = new NotAuthenticatedService();

    _serverSocket.connectionPending() += Pt::slot(*this, &Server::onAccept);
}


Server::Server(System::EventLoop& eventLoop)
: _loop(&eventLoop)
, _sslctx(0)
, _ssl(false)
, _useWorker(0)
, _maxThreads(1)
, _readTimeout(20000)
, _writeTimeout(20000)
, _keepAliveTimeout(30000)
{
    _notFoundService = new NotFoundService();
    _noAuthService = new NotAuthenticatedService();

    _serverSocket.setActive(*_loop);
    _serverSocket.connectionPending() += Pt::slot(*this, &Server::onAccept);
}


Server::Server(System::EventLoop& eventLoop, const std::string& ip, unsigned short int port, int backlog)
: _loop(&eventLoop)
, _serverSocket(ip, port, backlog)
, _sslctx(0)
, _ssl(false)
, _useWorker(0)
, _maxThreads(1)
, _readTimeout(20000)
, _writeTimeout(20000)
, _keepAliveTimeout(30000)
{
    _notFoundService = new NotFoundService();
    _noAuthService = new NotAuthenticatedService();

    _serverSocket.setActive(*_loop);
    _serverSocket.beginAccept();
    _serverSocket.connectionPending() += Pt::slot(*this, &Server::onAccept);

    this->startWorker();
}

Server::Server(System::EventLoop& eventLoop, const Pt::Net::AddrInfo& addr, int backlog)
: _loop(&eventLoop)
, _serverSocket(addr, backlog)
, _sslctx(0)
, _ssl(false)
, _useWorker(0)
, _maxThreads(1)
, _readTimeout(20000)
, _writeTimeout(20000)
, _keepAliveTimeout(30000)
{
    _notFoundService = new NotFoundService();
    _noAuthService = new NotAuthenticatedService();

    _serverSocket.setActive(*_loop);
    _serverSocket.beginAccept();
    _serverSocket.connectionPending() += Pt::slot(*this, &Server::onAccept);

    this->startWorker();
}

Server::~Server()
{
    this->shutdown();

    delete _notFoundService;
    delete _noAuthService;
}


void Server::setActive(System::EventLoop& eventLoop)
{
     _serverSocket.setActive(eventLoop);
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


void Server::shutdown()
{
    std::vector<ServerThread*>::iterator threadIt;
    for(threadIt = _serverThreads.begin(); threadIt != _serverThreads.end(); ++threadIt)
    {
        (*threadIt)->stop();
        delete *threadIt;
    }

    _serverThreads.clear();

    std::vector<RequestHandler*>::iterator it;
    for(it = _handlers.begin(); it != _handlers.end(); ++it)
    {
        delete *it;
    }

    _handlers.clear();

    ServiceMap::const_iterator srv;
    while( ! _services.empty() )
    {
        removeService( *(_services.begin()->second) );
    }
}


void Server::addService(const std::string& url, Service& service)
{
    service.registerServer(*this);

    System::WriteLock serviceLock(_serviceMutex);
    _services.insert( ServiceMap::value_type(url, &service) );
}


void Server::removeService(Service& service)
{
    // we can wait for shutdown by monitoring handler states in
    // onConnectionTimeout which handles destruction of all handlers

    std::vector<RequestHandler*>::iterator it;
    for(it = _handlers.begin(); it != _handlers.end(); ++it)
    {
        RequestHandler* rh = *it;
        if(rh->service() == &service)
        {
            delete rh;
        }
    }

    {
        System::WriteLock serviceLock(_serviceMutex);
        ServiceMap::iterator it = _services.begin();
        while( it != _services.end() )
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

    // TODO: simply delete all request handlers in each thread that
    //       use this service
    service.waitIdle();
    service.unregisterServer(*this);
}


void Server::setSecure(Ssl::Context& ctx)
{
    _ssl = true;
    _sslctx = &ctx;
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

    return _notFoundService->createResponder(request);
}


Service* Server::findService(const RequestHeader& request)
{
    System::ReadLock serviceLock(_serviceMutex);

    for (ServiceMap::const_iterator it = _services.lower_bound(request.url());
        it != _services.end() && it->first == request.url(); ++it)
    {
        /*if (!it->second->checkAuth(request))
        {
            return _noAuthService->createResponder(request, it->second->realm(), it->second->authContent());
        }*/

        return it->second;
    }

    return _notFoundService;
}


Responder* Server::notFoundResponder(const RequestHeader& request)
{ 
    return _notFoundService->createResponder(request); 
}


void Server::startWorker()
{
    for(unsigned n = 1; n < this->maxThreads(); ++n)
    {
        ServerThread* st = 0;

#ifdef PT_HTTP_WITH_SSL
        if(_ssl)
        {
            st = new ServerThread(*this, _sslctx);
        }
        else
#endif
            st = new ServerThread(*this);

        _serverThreads.push_back(st);
    }

    _useWorker = _serverThreads.size();
}


void Server::onAccept(Net::TcpServer& server)
{
    // TODO: we should only pass the TcpSocket to the worker thread
    //       so that a RequestHandler can be constructed with an event loop

    RequestHandler* conn = new RequestHandler(*this, server);

    if(_useWorker < _serverThreads.size())
    {
        _serverThreads[_useWorker]->serve(conn);
        ++_useWorker;
    }
    else
    {
        if(_sslctx)
            conn->setSecure(*_sslctx);
        
        conn->beginServe(*_loop);
        _handlers.push_back(conn);
        conn->timeout() += Pt::slot(*this, &Server::onConnectionTimeout);
        _useWorker = 0;
    }

    _serverSocket.beginAccept();
}


void Server::onConnectionTimeout(RequestHandler& conn)
{
    std::vector<RequestHandler*>::iterator it;
    for(it = _handlers.begin(); it != _handlers.end(); ++it)
    {
        if(&conn == *it)
        {
            delete *it;
            _handlers.erase(it);
            break;
        }
    }
}

} // namespace Http

} // namespace Pt
