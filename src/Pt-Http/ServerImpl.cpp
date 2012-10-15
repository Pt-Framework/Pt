/*
 * Copyright (C) 2011-2012 by Marc Boris Duerner
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

#include "ServerImpl.h"
#include <Pt/Http/Servlet.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/Responder.h>
#include <Pt/Http/Authentication.h>
#include <Pt/System/Logger.h>
#include <memory>
#include <cassert>

log_define("Pt.Http.Server")

namespace Pt {

namespace Http {

Acceptor::Acceptor(ServerImpl& server, Net::TcpServer& tcpServer)
: _server(server)
, _challenge(0)
, _servlet(0)
, _responder(0)
, _conn()
, _request(_conn)
, _reply(_conn)
{
    _conn.accept(tcpServer);
    _request.inputReceived() += Pt::slot(*this, &Acceptor::onRequestReceived);
    _reply.outputSent() += Pt::slot(*this, &Acceptor::onReplySent);
}


Acceptor::~Acceptor()
{
    releaseResponder();
    
    if(_challenge)
    {
        assert(_servlet);
        assert(_servlet->authentication());
        _servlet->authentication()->cancelChallenge(_challenge);
    }
}


void Acceptor::releaseResponder()
{
    log_trace("Acceptor::releaseResponder " << _responder);
    if( _responder )
    {
        assert(_servlet);
        _servlet->service()->releaseResponder(_responder);
        _servlet = 0;
        _responder = 0;
    }
}


void Acceptor::beginServe(System::EventLoop& loop)
{  
    log_trace("Acceptor::beginServe");

    _conn.setActive(loop);

    _reply.clear();
    _request.clear();
    _request.beginReceive();
}


void Acceptor::onRequestReceived(Request& req)
{
    log_trace("Acceptor::onRequestReceived");

    // TODO: error reply on HTTP related exceptions
    // replyError();
    
    try
    {
        MessageProgress progress = _request.endReceive();
        
        if( progress.header() )
        {
            log_debug("received request header");

            assert(_servlet == 0);
            _servlet = _server.getServlet(_request);
            if( ! _servlet )
            {
                _reply.setStatus(404, "Not found");
                _reply.beginSend();
                return;
            }

            Authentication* authentication = _servlet->authentication();
            if( authentication )
            {
                log_debug("authentication required");

                // TODO: only authenticate if we haven't already for this connection
                bool granted = authentication->authenticate(_request, _reply);
                if( ! granted )
                {
                    _challenge = authentication->beginChallenge(_request, _reply);
                    if( ! _challenge)
                    {
                        log_debug("request immediately denied");

                        if( ! _reply.isSending() )
                            _reply.beginSend(true);

                        _servlet = 0;
                    }
                    else
                    {
                        log_debug("authentication started");
                        _requestProgress = progress;
                        _challenge->finished() += Pt::slot(*this, &Acceptor::onChallenge);
                    }

                    return;
                }

                log_debug("request immediately authenticated");
            }
        
            assert(_responder == 0);
            _responder = _servlet->service()->getResponder( _request );
            
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
            if( ! this->onRequestBody() )
                return;
        }

        if( progress.finished() )
        {
            if( ! this->onRequestFinished() )
                return;
        }

        log_debug("read request");
        _request.beginReceive();
    }
    catch(const System::IOError& e) // TODO: HttpError is also an IOError
    {
        log_error("EXCEPTION: " << e.what());
        _finished.send(*this);
    }
}


void Acceptor::onChallenge(Challenge& challenge)
{
    log_trace("Acceptor::onChallenge");

    try
    {
        bool granted = _servlet->authentication()->endChallenge(_challenge, _request, _reply);

        _challenge = 0;
    
        if( ! granted )
        {
            log_debug("request not granted");

            if( ! _reply.isSending() )
                _reply.beginSend(true);

            _servlet = 0;
        }
        else
        {
            assert(_responder == 0);
            _responder = _servlet->service()->getResponder( _request );
            
            assert(_responder);
            _responder->beginRequest( _request, _reply );

            if( _reply.isSending() )
            {
                log_debug("request interrupted");
                return;
            }

            if( _requestProgress.body() )
            {     
                if( ! this->onRequestBody() )
                    return;
            }

            if( _requestProgress.finished() )
            {
                if( ! this->onRequestFinished() )
                    return;
            }

            log_debug("read request");
            _request.beginReceive();
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


bool Acceptor::onRequestBody()
{      
    if( _responder)
    {
        log_debug("reading request");
        _responder->readRequest(_request, _reply);

        if( _reply.isSending() )
        {
            log_debug("request interrupted");
            return false;
        }
    }
    else
    {
        log_debug("ignoring request body");
        _request.body().discard();
    }

    return true;
}


bool Acceptor::onRequestFinished()
{
    if( ! _conn.isConnected() )
    {
        log_debug("not connected anymore");
        _finished.send(*this);
        return false;
    }

    if(_responder)
    {
        log_debug("request body finished, begin reply");
        _responder->beginReply(_request, _reply);
        return false;
    }

    // if there is no responder, the reply was already sent, because a 
    // request, interrupted at an early stage, was just finished
    return true;
}


void Acceptor::onReplySent(Reply& r)
{
    log_trace("Acceptor::onReplySent");

    try
    {
        MessageProgress progress = _reply.endSend();

        if( ! progress.finished() )
        {
            log_debug("writing more reply data");
            bool finished = _reply.isFinished();
            _reply.beginSend(finished);
            return;
        }

        if( ! _reply.isFinished() )
        {
            log_debug("continuing response");
            _reply.body().discard();
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


void Acceptor::replyError()
{
    _reply.clear();

    _reply.setStatus(500, "internal server error");
    _reply.header().set("Content-Type", "text/plain");
    _reply.header().set("Connection", "close");
    _reply.body() << "Error 500: Internal server error.";

    _reply.beginSend(true);
}




ServerThread::ServerThread()
: _ssl(false)
, _thread(_loop)
, _isReturned(false)
, _isServletIdle(false)
{
    _loop.event() += Pt::slot(*this, &ServerThread::onAccept);
    _loop.event() += Pt::slot(*this, &ServerThread::onRemoveServlet);
    _loop.event() += Pt::slot(*this, &ServerThread::onIsServletIdle);
    _thread.start();
}


ServerThread::~ServerThread()
{
    stop();
}


void ServerThread::setSecure(Ssl::Context& ctx)
{
#ifdef PT_HTTP_WITH_SSL
    _sslctx.assign(ctx);
    _ssl = true;
#endif
}
        

void ServerThread::serve(Acceptor* conn)
{
    AcceptEvent ev(conn);
    _loop.commitEvent(ev);
}


void ServerThread::stop()
{
    _loop.exit();
    _thread.join();

    std::vector<Acceptor*>::iterator it;
    for(it = _handlers.begin(); it != _handlers.end(); ++it)
    {
        delete *it;
    }

    _handlers.clear();
}


void ServerThread::removeServlet(Servlet& servlet)
{
    RemoveServletEvent ev(&servlet);
    _loop.commitEvent(ev);

    System::MutexLock lock(_invokeMutex);
    _isReturned = false;

    while( ! _isReturned)
        _hasReturned.wait(lock);
}


bool ServerThread::isServletIdle(Servlet& servlet)
{
    ServletIdleEvent ev(&servlet);
    _loop.commitEvent(ev);

    System::MutexLock lock(_invokeMutex);

    _isServletIdle = false;
    _isReturned = false;

    while( ! _isReturned)
        _hasReturned.wait(lock);

    return _isServletIdle;
}


void ServerThread::onAccept(const AcceptEvent& ev)
{
    Acceptor* handler = ev.connection();

    _handlers.push_back(handler);
    handler->finished() += Pt::slot(*this, &ServerThread::onHandlerFinished);

#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
        handler->setSecure(_sslctx);
#endif

    handler->beginServe(_loop);
}


void ServerThread::onRemoveServlet(const RemoveServletEvent& ev)
{
    std::vector<Acceptor*>::iterator it  = _handlers.begin();
    while( it != _handlers.end() )
    {
        Acceptor* rh = *it;
        if( rh->servlet() == ev.servlet() )
        {
            delete rh;
            it = _handlers.erase(it);
        }
        else
        {
            ++it;
        }
    }

    System::MutexLock lock(_invokeMutex);
    _isReturned = true;
    _hasReturned.signal();
}


void ServerThread::onIsServletIdle(const ServletIdleEvent& ev)
{
    std::vector<Acceptor*>::iterator it;
    for( it  = _handlers.begin(); it != _handlers.end(); ++it )
    {
        if( (*it)->servlet() == ev.servlet() )
        {
            break;
        }
    }

    System::MutexLock lock(_invokeMutex);
    _isReturned = true;
    _isServletIdle = (it == _handlers.end());
    _hasReturned.signal();
}


void ServerThread::onHandlerFinished(Acceptor& handler)
{
    std::vector<Acceptor*>::iterator it;
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




ServerImpl::ServerImpl()
: _sslctx(0)
, _useWorker(0)
, _timeout(30000)
, _keepAliveTimeout(30000)
{
    _serverSocket.connectionPending() += Pt::slot(*this, &ServerImpl::onAccept);
}


ServerImpl::~ServerImpl()
{
    this->cancel();

    while( ! _servlets.empty() )
    {
        _servlets.front().servlet()->detach();
    }
}


void ServerImpl::listen(const Pt::Net::AddrInfo& addr, const Server::Options& options)
{
    std::vector<ServerThread*>::iterator thread;
    for(thread = _serverThreads.begin(); thread != _serverThreads.end(); ++thread)
    {
        (*thread)->stop();
        delete *thread;
    }

    _serverThreads.clear();

#ifdef PT_HTTP_WITH_SSL
        _sslctx = options.sslContext();
#endif

    _serverSocket.listen(addr, options.tcpOptions());
    _serverSocket.beginAccept();

    for(unsigned n = 1; n < options.maxThreads(); ++n)
    {
        ServerThread* st = new ServerThread();

#ifdef PT_HTTP_WITH_SSL
        if(_sslctx)
            st->setSecure(*_sslctx);
#endif

        _serverThreads.push_back(st);
    }

    _useWorker = _serverThreads.size();
}


void ServerImpl::cancel()
{
    _serverSocket.cancel();

    std::vector<ServerThread*>::iterator threadIt;
    for(threadIt = _serverThreads.begin(); threadIt != _serverThreads.end(); ++threadIt)
    {
        (*threadIt)->stop();
        delete *threadIt;
    }

    _serverThreads.clear();

    std::vector<Acceptor*>::iterator it;
    for(it = _handlers.begin(); it != _handlers.end(); ++it)
    {
        delete *it;
    }

    _handlers.clear();
}


void ServerImpl::addServlet(Servlet& servlet)
{
    System::WriteLock serviceLock(_serviceMutex);
    ServletListEntry entry(&servlet);
    _servlets.push_back(entry);
}


void ServerImpl::removeServlet(Servlet& servlet)
{
    // remove the servlet from the list, so no new connection can use it
    System::WriteLock serviceLock(_serviceMutex);

    ServletList::iterator it;
    for(it = _servlets.begin(); it != _servlets.end(); ++it)
    {
        if(it->servlet() == &servlet)
        {
            _servlets.erase(it);
            break;
        }
    }

    serviceLock.unlock();

    // close all connections in this thread, which use the servlet
    std::vector<Acceptor*>::iterator hit  = _handlers.begin();
    while( hit != _handlers.end() )
    {
        std::vector<Acceptor*>::iterator handler = hit++;
        
        if( (*handler)->servlet() == &servlet )
        {
            delete *handler;
            hit = _handlers.erase(handler);
        }
    }

    // close all connections in the worker threads which use the servlet
    std::vector<ServerThread*>::iterator threadIt;
    for(threadIt = _serverThreads.begin(); threadIt != _serverThreads.end(); ++threadIt)
    {
        // returns when all connections using the service are closed
        (*threadIt)->removeServlet(servlet);
    }

    //NOTE: in case of an exception, terminate the worker thread
}


void ServerImpl::shutdownServlet(Servlet& servlet, bool shutdown)
{
    System::WriteLock serviceLock(_serviceMutex);

    ServletList::iterator it;
    for(it = _servlets.begin(); it != _servlets.end(); ++it)
    {
        if(it->servlet() == &servlet)
        {
            it->setShutdown(shutdown);
            break;
        }
    }
}


bool ServerImpl::isServletIdle(Servlet& servlet)
{
    // check all connections in this thread
    std::vector<Acceptor*>::iterator it;
    for( it = _handlers.begin(); it != _handlers.end(); ++it)
    {      
        if( (*it)->servlet() == &servlet )
        {
            return false;
        }
    }

    // check all worker threads
    std::vector<ServerThread*>::iterator thread;
    for(thread = _serverThreads.begin(); thread != _serverThreads.end(); ++thread)
    {
        // returns when all connections using the service are closed
        bool idle = (*thread)->isServletIdle(servlet);

        if( ! idle)
            return false;
    }

    return true;
}


Servlet* ServerImpl::getServlet(const Request& request)
{
    System::ReadLock serviceLock(_serviceMutex);

    for(ServletList::iterator it = _servlets.begin(); it != _servlets.end(); ++it)
    {
        if( it->isShutdown() )
            continue;

        if( ! it->servlet()->isMapped(request) )
            continue;

        log_info("serving: " << request.url());
        return it->servlet();
    }

    log_warn("not found: " << request.url());
    return 0;
}


void ServerImpl::onAccept(Net::TcpServer& server)
{
    log_trace("Server::onAccept");

    // TODO: we should only pass the TcpSocket to the worker thread so that 
    // an Acceptor can be constructed with an event loop there

    std::auto_ptr<Acceptor> handler( new Acceptor(*this, server) );

    log_debug("handler timeouts: " << _timeout << ", " << _keepAliveTimeout);
    handler->setTimeout(_timeout);
    handler->setKeepAliveTimeout(_keepAliveTimeout);

    if( _useWorker < _serverThreads.size() ) // worker thread
    {
        _serverThreads[_useWorker]->serve( handler.release() );
        ++_useWorker;
    }
    else // this thread
    {
        if(_sslctx)
            handler->setSecure(*_sslctx);
        
        System::EventLoop* loop = this->loop();
        if( ! loop)
        {
            // NOTE: this can not really happen, because the signal is only
            // sent when a loop is present
            throw std::logic_error("http server has no event loop");
        }


        handler->beginServe(*loop);
        handler->finished() += Pt::slot(*this, &ServerImpl::onHandlerFinished);
        _handlers.push_back( handler.get() );
        handler.release();

        _useWorker = 0;
    }

    _serverSocket.beginAccept();
}


void ServerImpl::onHandlerFinished(Acceptor& h)
{
    std::vector<Acceptor*>::iterator it;
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
