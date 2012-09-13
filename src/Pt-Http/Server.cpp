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
        { _conn.setSecure(ctx); }

        void setTimeout(std::size_t timeout)
        { _conn.setTimeout(timeout); }

        void setKeepAliveTimeout(std::size_t timeout)
        { _conn.setKeepAliveTimeout(timeout); }

        void beginServe(System::EventLoop& loop);

        Signal<RequestHandler&>& finished()
        { return _finished; }

        Service* service()
        { 
            return _responder ? &_responder->service() : 0; 
        }

    protected:
        Responder& getResponder(const RequestHeader& request);

        void releaseResponder();

        void onRequestReceived(Request& req);

        void onReplySent(Reply& r);

        void replyError();

    private:
        Server& _server;
        Responder* _responder;
        Connection _conn;
        Request _request;
        Reply _reply;
        MessageProgress _requestProgress;
        Signal<RequestHandler&> _finished;
};


RequestHandler::RequestHandler(Server& server, Net::TcpServer& tcpServer)
: _server(server)
, _responder(0)
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
}


Responder& RequestHandler::getResponder(const RequestHeader& request)
{ 
    _responder = _server.getResponder(request);
    assert(_responder);

    return *_responder;
}


void RequestHandler::releaseResponder()
{
    if( _responder )
    {
        Service& service = _responder->service();
        service.releaseResponder(_responder);
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


void RequestHandler::onRequestReceived(Request& req)
{
    log_trace("RequestHandler::onRequestReceived");

    // TODO: error reply on HTTP exception
    //_responder = _server.getDefaultResponder(_request->header());
    //replyError();
    
    try
    {
        MessageProgress progress = _request.endReceive();

        if( progress.header() )
        {
            log_debug("received request header");
            //Authentication* _authorization = 0;
            Responder& responder = getResponder( _request.header() );

            //if(_authorization)
            //{
            //    // if NULL access is denied at once
            //    _challenge = auth->authenticate(_request, _reply);

            //    if(_challenge)
            //    {
            //        _requestProgress = progress;
            //        _challenge->beginExecute(_request);
            //    }
            //}

            responder.beginRequest( _request, _reply );

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

            // only reply if not leftover body from interrupted request
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
    catch(const System::IOError& e) // TODO: HttpError is also an IOError
    {
        log_error("EXCEPTION: " << e.what());
        _finished.send(*this);
    }
}


void RequestHandler::onReplySent(Reply& r)
{
    log_trace("RequestHandler::onReplySent");

    try
    {
        MessageProgress progress = _reply.endSend();

        if( ! progress.finished() )
        {
            log_debug("writing left over data");
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
        releaseResponder();
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
    _noAuthService = new NotAuthenticatedService();

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
, _timeout(30000)
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
, _timeout(30000)
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
    this->cancel();

    while( ! _services.empty() )
    {
        Service* service = _services.begin()->first;
        unregisterService(*service);
    }

    delete _notFoundService;
    delete _noAuthService;
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


void Server::registerService(MapService* m, Service& service)
{
    ServiceMap::value_type elem(&service, m);

    System::WriteLock serviceLock(_serviceMutex);
    _services.insert(elem);
    service.registerServer(*this);
}


void Server::unregisterService(Service& service)
{
    System::WriteLock serviceLock(_serviceMutex);
    _services.erase(&service);
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


Responder* Server::getResponder(const RequestHeader& request)
{
    System::ReadLock serviceLock(_serviceMutex);

    Responder* responder = 0;
    for(ServiceMap::const_iterator it = _services.begin(); it != _services.end(); ++it)
    {
        if( ! it->second->map(request) )
            continue;

        if( ! it->first->checkAuth(request) )
        {
            responder = _noAuthService->getResponder(request);
            return responder;
        }

        responder = it->first->getResponder(request);
        if(responder)
            return responder;
    }

    log_error("not found: " << request.url());
    responder = _notFoundService->getResponder(request);
    return responder;
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
    // TODO: we should only pass the TcpSocket to the worker thread
    //       so that a RequestHandler can be constructed with an event loop

    RequestHandler* handler = new RequestHandler(*this, server);
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
