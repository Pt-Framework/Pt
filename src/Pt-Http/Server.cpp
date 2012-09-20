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
#include <Pt/Http/Server.h>
#include <Pt/Http/Authentication.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/Responder.h>

#ifdef PT_HTTP_WITH_SSL
#include <Pt/Ssl/Context.h>
#endif

#include <Pt/System/MainLoop.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Condition.h>
#include <Pt/System/Logger.h>

#include <memory>
#include <cassert>

log_define("Pt.Http.Server")

namespace Pt {

namespace Http {

class RequestHandler : public Pt::Connectable
{
    struct DeferRelease
    {
        DeferRelease(RequestHandler* r)
        : _service(0)
        , _responder(0)
        , _r(r)
        {
            _r->_deferRelease = this;
        }

        ~DeferRelease()
        {
            if(_service)
            {
                _service->releaseResponder(_responder);
            }
            else
                _r->_deferRelease = 0;
        }

        void set(Service* service, Responder* responder)
        {
            _service = service;
            _responder = responder;
            _r->_deferRelease = 0;
            _r->_service = 0;
            _r->_responder = 0;
        }

        Service* _service;
        Responder* _responder;
        RequestHandler* _r;
    };

    public:
        RequestHandler(Server& server, Net::TcpServer& tcpServer);

        ~RequestHandler();

        void setSecure(Ssl::Context& ctx)
        { _conn.setSecure(ctx); }

        void setTimeout(std::size_t timeout)
        { _conn.setTimeout(timeout); }

        void setKeepAliveTimeout(std::size_t timeout)
        { _conn.setKeepAliveTimeout(timeout); }

        void beginServe(System::EventLoop& loop);

        Signal<RequestHandler&>& finished()
        { return _finished; }

        Service* service()
        { return _service; }

        Authentication* authentication()
        { return _authentication; }

    protected:
        void releaseResponder();

        void onChallenge(Challenge& challenge);

        void onRequestReceived(Request& req);

        void onRequestProgress(MessageProgress progress);

        void onReplySent(Reply& r);

        void replyError();

    private:
        Server& _server;
        Authentication* _authentication;
        Challenge* _challenge;
        Service* _service;
        Responder* _responder;
        DeferRelease* _deferRelease;
        Connection _conn;
        Request _request;
        Reply _reply;
        MessageProgress _requestProgress;
        Signal<RequestHandler&> _finished;
};

RequestHandler::RequestHandler(Server& server, Net::TcpServer& tcpServer)
: _server(server)
, _authentication(0)
, _challenge(0)
, _service(0)
, _responder(0)
, _deferRelease(0)
, _conn()
, _request(_conn)
, _reply(_conn)
{
    _conn.accept(tcpServer);
    _request.inputReceived() += Pt::slot(*this, &RequestHandler::onRequestReceived);
    _reply.outputSent() += Pt::slot(*this, &RequestHandler::onReplySent);
}


RequestHandler::~RequestHandler()
{
    releaseResponder();
    
    if(_challenge)
    {
        assert(_authentication);
        _authentication->cancelAuthenticate(_challenge);
    }
}


void RequestHandler::releaseResponder()
{
    log_trace("RequestHandler::releaseResponder " << _responder);
    if( _responder )
    {
        assert(_service);
        _service->releaseResponder(_responder);
        _service = 0;
        _responder = 0;
    }
}


void RequestHandler::beginServe(System::EventLoop& loop)
{  
    log_trace("RequestHandler::beginServe");

    _conn.setActive(loop);

    _reply.clear();
    _request.clear();
    _request.beginReceive();
}


void RequestHandler::onChallenge(Challenge& challenge)
{
    log_trace("RequestHandler::onChallenge");

    try
    {
        bool granted = _authentication->endAuthenticate(_challenge, _request, _reply);

        _challenge = 0;
        _authentication = 0;
    
        if( ! granted )
        {
            log_debug("request not granted");
        
            if( ! _reply.isFinished() )
                _reply.finish();

            if( ! _reply.isSending() )
                _reply.beginSend();

            _service = 0;
        }
        else
        {
            _responder = _service->getResponder( _request.header() );
            assert(_responder);
            _responder->beginRequest( _request, _reply );

            if( _reply.isSending() )
            {
                log_debug("request interrupted");
                return;
            }

            onRequestProgress(_requestProgress);
        }
    }
    catch(const System::IOError& e) // TODO: HttpError is also an IOError
    {
        log_error("EXCEPTION: " << e.what());

        // TODO: error reply on HTTP related exceptions
        // replyError();
        _finished.send(*this);
    }
}


void RequestHandler::onRequestReceived(Request& req)
{
    log_trace("RequestHandler::onRequestReceived");

    // TODO: error reply on HTTP related exceptions
    // replyError();
    
    try
    {
        MessageProgress progress = _request.endReceive();
        onRequestProgress(progress);
    }
    catch(const System::IOError& e) // TODO: HttpError is also an IOError
    {
        log_error("EXCEPTION: " << e.what());
        _finished.send(*this);
    }
}


void RequestHandler::onRequestProgress(MessageProgress progress)
{
    log_trace("RequestHandler::onRequestProgress");

    if( progress.header() )
    {
        log_debug("received request header");

        assert(_authentication == 0);
        assert(_challenge == 0);
        assert(_service == 0);

        _service = _server.getService( _request.header(), _authentication );

        if(_authentication)
        {
            log_debug("authentication required");

            // TODO: only authenticate if we haven't already for this connection

            _challenge = _authentication->beginAuthenticate(_request, _reply);
            if(_challenge)
            {
                log_debug("authentication started");
                _requestProgress = progress;
                _requestProgress.unsetHeader();

                _challenge->finished() += Pt::slot(*this, &RequestHandler::onChallenge);
                _challenge->beginVerify();
                return;
            }

            log_debug("request immediately authenticated");
            _authentication = 0;
        }
        
        _responder = _service->getResponder( _request.header() );
        assert(_responder);
        _responder->beginRequest( _request, _reply );

        if( _reply.isSending() )
        {
            log_debug("request interrupted");
            return;
        }
    }
    
    if( progress.body() )
    {     
        if( _responder)
        {
            log_debug("reading request");
            _responder->readRequest(_request, _reply);

            if( _reply.isSending() )
            {
                log_debug("request interrupted");
                return;
            }
        }
        else
        {
            log_debug("ignoring request body");
            _request.clearBody();
        }
    }

    if( progress.finished() )
    {
        if( ! _conn.isConnected() )
        {
            log_debug("not connected anymore");
            _finished.send(*this);
            return;
        }

        if(_responder)
        {
            log_debug("request body finished, begin reply");
            
            DeferRelease deferRelease(this);
            _responder->beginReply(_request, _reply);
            
            log_debug("reply started");
            return;
        }

        // if there is no responder, the reply was already sent and a request
        // interrupted at an early stage was just finished
    }

    log_debug("read request");
    _request.beginReceive();
}


void RequestHandler::onReplySent(Reply& r)
{
    log_trace("RequestHandler::onReplySent");

    try
    {
        MessageProgress progress = _reply.endSend();

        if( ! progress.finished() )
        {
            log_debug("writing more reply data");
            _reply.beginSend();
            return;
        }

        if( ! _reply.isFinished() )
        {
            log_debug("continuing response");
            _reply.clearBody();
            assert(_responder);
            _responder->writeReply(_request, _reply);
            return;
        }

        log_debug("response finished");

        if(_deferRelease)
        {
            log_debug("defer responder release");
            assert(_service);
            assert(_responder);
            _deferRelease->set(_service, _responder);
        }
        else
        {
            releaseResponder();
        }

        _reply.clear();
        _request.clear();

        if( ! _conn.isConnected() )
        {
            log_debug("not connected anymore");
            _finished.send(*this);
            return;
        }

        _request.beginReceive();
    }
    catch(const System::IOError& e) // TODO: HttpError is also an IOError
    {
        log_error("EXCEPTION: " << e.what());
        _finished.send(*this);
    }
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

        class RemoveServiceEvent : public Pt::BasicEvent<RemoveServiceEvent>
        {
            public:
                RemoveServiceEvent(Service* s)
                : _service(s)
                { }
    
                Service* service() const
                { return _service; }
    
            private:
                Service* _service;
        };

    public:
        ServerThread(Server& server)
        : _server(&server)
        , _ssl(false)
        , _thread(_loop)
        , _removed(false)
        {
            _loop.event() += Pt::slot(*this, &ServerThread::onAccept);
            _loop.event() += Pt::slot(*this, &ServerThread::onRemoveService);
            _thread.start();
        }

        ~ServerThread()
        {
            stop();
        }

        void setSecure(Ssl::Context& ctx)
        {
#ifdef PT_HTTP_WITH_SSL
            _sslctx.assign(ctx);
            _ssl = true;
#endif
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
            for(it = _handlers.begin(); it != _handlers.end(); ++it)
            {
                delete *it;
            }

            _handlers.clear();
        }

        void removeService(Service& service)
        {
            RemoveServiceEvent ev(&service);
            _loop.commitEvent(ev);

            System::MutexLock lock(_removedMutex);
            _removed = false;

            while( ! _removed)
                _isRemoved.wait(lock);
        }

    private:
        void onAccept(const AcceptEvent& ev)
        {
            RequestHandler* handler = ev.connection();

            _handlers.push_back(handler);
            handler->finished() += Pt::slot(*this, &ServerThread::onHandlerFinished);

#ifdef PT_HTTP_WITH_SSL
            if(_ssl)
                handler->setSecure(_sslctx);
#endif

            handler->beginServe(_loop);
        }

        void onRemoveService(const RemoveServiceEvent& ev)
        {
            std::vector<RequestHandler*>::iterator it  = _handlers.begin();
            while( it != _handlers.end() )
            {
                RequestHandler* rh = *it;
                if( rh->service() == ev.service() )
                {
                    delete rh;
                    it = _handlers.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            System::MutexLock lock(_removedMutex);
            _removed = true;
            _isRemoved.signal();
        }

        void onHandlerFinished(RequestHandler& handler)
        {
            std::vector<RequestHandler*>::iterator it;
            for(it = _handlers.begin(); it != _handlers.end(); ++it)
            {
                if(&handler == *it)
                {
                    delete *it;
                    _handlers.erase(it);
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
        std::vector<RequestHandler*> _handlers;

        bool _removed;
        System::Mutex _removedMutex;
        System::Condition _isRemoved;
};


Server::Server()
: _loop(0)
, _sslctx(0)
, _ssl(false)
, _useWorker(0)
, _maxThreads(1)
, _timeout(30000)
, _keepAliveTimeout(30000)
{
    _notFoundService = new NotFoundService();

    _serverSocket.connectionPending() += Pt::slot(*this, &Server::onAccept);
}


Server::Server(System::EventLoop& eventLoop)
: _loop(&eventLoop)
, _sslctx(0)
, _ssl(false)
, _useWorker(0)
, _maxThreads(1)
, _timeout(30000)
, _keepAliveTimeout(30000)
{
    _notFoundService = new NotFoundService();

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
, _timeout(30000)
, _keepAliveTimeout(30000)
{
    _notFoundService = new NotFoundService();

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
, _timeout(30000)
, _keepAliveTimeout(30000)
{
    _notFoundService = new NotFoundService();

    _serverSocket.setActive(*_loop);
    _serverSocket.beginAccept();
    _serverSocket.connectionPending() += Pt::slot(*this, &Server::onAccept);

    this->startWorker();
}

Server::~Server()
{
    this->cancel();

    while( ! _services.empty() )
    {
        Service* service = _services.begin()->service;
        unregisterService(*service);
    }

    delete _notFoundService;
}


void Server::setActive(System::EventLoop& eventLoop)
{
     _serverSocket.setActive(eventLoop);
}


void Server::setSecure(Ssl::Context& ctx)
{
    _ssl = true;
    _sslctx = &ctx;
}


std::size_t Server::timeout() const
{
    return _timeout;
}


void Server::setTimeout(std::size_t ms)
{
    _timeout = ms;
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


void Server::listen(const Pt::Net::AddrInfo& addr, int backlog)
{
    _serverSocket.listen(addr, backlog);
    _serverSocket.beginAccept();

    this->startWorker();
}


void Server::listen(const std::string& ip, unsigned short int port, int backlog)
{
    _serverSocket.listen(ip, port, backlog);
    _serverSocket.beginAccept();

    this->startWorker();
}


void Server::cancel()
{
    _serverSocket.cancel();

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
}


void Server::registerService(MapService* m, Service& service, Authentication* auth)
{
    Servlet s(SmartPtr<MapService>(m), service, auth);
    
    System::WriteLock serviceLock(_serviceMutex);
    _services.push_back(s);
    service.registerServer(*this);
}


void Server::unregisterService(Service& service)
{
    System::WriteLock serviceLock(_serviceMutex);

    for(unsigned n = 0; n < _services.size(); )
    {
        if(_services[n].service == &service)
        {
            ServiceMap::iterator it = _services.begin() + n;
            _services.erase(it);
            continue;
        }
    
        ++n;
    }

    service.unregisterServer(*this);
}


void Server::removeService(Service& service)
{
    // remove service, so no responders can be created anymore
    unregisterService(service);

    // close all connections in this thread, which use the service
    std::vector<RequestHandler*>::iterator hit  = _handlers.begin();
    while( hit != _handlers.end() )
    {
        std::vector<RequestHandler*>::iterator handler = hit++;
        
        if( (*handler)->service() == &service )
        {
            delete *handler;
            hit = _handlers.erase(handler);
        }
    }

    // close all connections in the worker threads which use the service
    std::vector<ServerThread*>::iterator threadIt;
    for(threadIt = _serverThreads.begin(); threadIt != _serverThreads.end(); ++threadIt)
    {
        // returns when all connections using the service are closed
        (*threadIt)->removeService(service);
    }

    //NOTE: in case of an exception, terminate the worker thread
}


Service* Server::getService(const RequestHeader& request, Authentication*& auth)
{
    System::ReadLock serviceLock(_serviceMutex);

    for(ServiceMap::iterator it = _services.begin(); it != _services.end(); ++it)
    {
        if( ! it->mapper->map(request) )
            continue;

        auth = it->auth;
        return it->service;
    }

    log_warn("not found: " << request.url());
    auth = 0;
    return _notFoundService;
}


void Server::startWorker()
{
    for(unsigned n = 1; n < this->maxThreads(); ++n)
    {
        ServerThread* st = new ServerThread(*this);

#ifdef PT_HTTP_WITH_SSL
        if(_ssl)
        {
            st->setSecure(*_sslctx);
        }
#endif

        _serverThreads.push_back(st);
    }

    _useWorker = _serverThreads.size();
}


void Server::onAccept(Net::TcpServer& server)
{
    log_trace("Server::onAccept");

    // TODO: we should only pass the TcpSocket to the worker thread
    //       so that a RequestHandler can be constructed with an event loop

    RequestHandler* handler = new RequestHandler(*this, server);

    log_debug("handler timeouts: " << _timeout << ", " << _keepAliveTimeout);
    handler->setTimeout(_timeout);
    handler->setKeepAliveTimeout(_keepAliveTimeout);

    if(_useWorker < _serverThreads.size())
    {
        _serverThreads[_useWorker]->serve(handler);
        ++_useWorker;
    }
    else
    {
        if(_ssl)
            handler->setSecure(*_sslctx);
        
        handler->beginServe(*_loop);
        _handlers.push_back(handler);
        handler->finished() += Pt::slot(*this, &Server::onHandlerFinished);
        _useWorker = 0;
    }

    _serverSocket.beginAccept();
}


void Server::onHandlerFinished(RequestHandler& h)
{
    std::vector<RequestHandler*>::iterator it;
    for(it = _handlers.begin(); it != _handlers.end(); ++it)
    {
        if(&h == *it)
        {
            delete *it;
            _handlers.erase(it);
            break;
        }
    }
}

} // namespace Http

} // namespace Pt
