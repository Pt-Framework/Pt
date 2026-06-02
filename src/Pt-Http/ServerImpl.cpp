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

#include <Pt/SmartPtr.h>
#include <Pt/Http/Servlet.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/Responder.h>
#include <Pt/Http/Authorizer.h>
#include <Pt/Http/HttpError.h>
#include <Pt/System/Logger.h>

#include <limits>
#include <memory>
#include <cassert>

PT_LOG_DEFINE("Pt.Http.Server")

namespace Pt {

namespace Http {

Acceptor::Acceptor(ServerImpl& server, Net::TcpServer& tcpServer)
: _server(server)
, _state(OnBeginRequest)
, _auth(0)
, _servlet(0)
, _responder(0)
, _conn(new Connection())
, _request(*_conn)
, _reply(*_conn)
, _isReply(false)
{
    _conn->accept(tcpServer);
    _request.inputReceived() += Pt::slot(*this, &Acceptor::onRequestReceived);
    _reply.outputSent() += Pt::slot(*this, &Acceptor::onReplySent);
}


Acceptor::~Acceptor()
{
    releaseResponder();
    
    if(_auth)
    {
        assert(_servlet);
        assert(_servlet->authorizer());
        _servlet->authorizer()->cancelAuthorization(_auth);
    }

    if (_conn)
        delete _conn;
}


void Acceptor::releaseResponder()
{
    PT_LOG_TRACE("Acceptor::releaseResponder " << _responder);
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
    PT_LOG_TRACE("Acceptor::beginServe");

    _conn->setActive(loop);

    _reply.clear();
    _request.clear();
    _request.beginReceive();
}


void Acceptor::onRequestReceived(Request& req)
{
    PT_LOG_TRACE("Acceptor::onRequestReceived");
    
    try
    {
        _requestProgress = _request.endReceive();
        
        if( _requestProgress.header() )
        {
            PT_LOG_DEBUG("received request header");

            assert(_servlet == 0);
            _servlet = _server.getServlet(_request);
            if( ! _servlet )
            {
                _reply.setStatus(404, "Not found");
                _reply.beginSend();
                return;
            }

            Authorizer* authorizer = _servlet->authorizer();
            if( authorizer )
            {
                PT_LOG_DEBUG("authorization required");

                bool granted = false;
                _auth = authorizer->beginAuthorize(_request, _reply, granted);
                if(_auth)
                {
                    PT_LOG_DEBUG("authorization started");
                    _auth->beginAuthorize(_request, _reply);
                    _auth->finished() += Pt::slot(*this, &Acceptor::onAuthorization);
                    return;
                }
                
                if( ! granted )
                {
                    PT_LOG_DEBUG("access immediately denied");

                    if( ! _reply.isSending() )
                        _reply.beginSend(true);

                    _servlet = 0;
                    return;
                }

                PT_LOG_DEBUG("access immediately granted");
            }
        }
    
        onRequest(_requestProgress);
    }
    catch(const HttpError& e)
    {
        PT_LOG_WARN("EXCEPTION: " << e.what());

        replyError();
        _finished.send(*this);
    }
    catch(const System::IOError& e)
    {
        // this also catches keep-alive timeouts

        PT_LOG_WARN("EXCEPTION: " << e.what());
        _finished.send(*this);
    }
}


void Acceptor::onAuthorization(Authorization& auth)
{
    PT_LOG_TRACE("Acceptor::onAuthorization");

    try
    {
        bool granted = _servlet->authorizer()->endAuthorization(_auth);
        _auth = 0;
    
        if( ! granted )
        {
            PT_LOG_DEBUG("request not granted");

            if( ! _reply.isSending() )
                _reply.beginSend(true);

            _servlet = 0;
        }
        else
        {
            onRequest(_requestProgress);
        }
    }
    catch(const HttpError& e)
    {
        PT_LOG_WARN("EXCEPTION: " << e.what());

        replyError();
        _finished.send(*this);
    }
    catch(const System::IOError& e) 
    {
        PT_LOG_WARN("EXCEPTION: " << e.what());
        _finished.send(*this);
    }
}


void Acceptor::onRequest(MessageProgress progress)
{
    PT_LOG_TRACE("Acceptor::onRequest");

    try
    {
        if( progress.header() )
        {
            PT_LOG_DEBUG("received request header");

            _state = OnBeginRequest;

            assert(_responder == 0);
            _responder = _servlet->service()->getResponder( _request );
            assert(_responder);

            _responder->setAcceptor(*this);
            _responder->beginRequest( _request, _reply, *_conn->loop() );
            return;
        }
    
        if( progress.body() )
        {
            PT_LOG_DEBUG("received request body");

            _state = OnReadRequest;

            if( ! _responder )
            {
                // request was interrupted and remaining body will be ignored
                PT_LOG_DEBUG("ignoring request body");
                _request.discard();

                if( ! progress.finished() )
                {
                    PT_LOG_DEBUG("reading discarded request");
                    _request.beginReceive();
                }
                else
                {
                    _request.clear();
                }
            }
            else
            {
                _responder->readRequest( _request, _reply, *_conn->loop() );
                return;
            }
        }

        if( progress.finished() )
        {
            if( ! _conn->isConnected() )
            {
                PT_LOG_DEBUG("not connected anymore");
                _finished.send(*this);
                return;
            }

            if(_responder)
            {
                PT_LOG_DEBUG("request body finished, begin reply");

                _state = OnBeginReply;

                _responder->beginReply(_request, _reply, *_conn->loop());
                return;
            }

            // if there is no responder, the reply was finished before the request was
            // read completely. In this case the remaining request will be ignored
        }

        PT_LOG_DEBUG("continue reading request");
        _request.beginReceive();
    }
    catch(const System::IOError& e) 
    {
        PT_LOG_WARN("EXCEPTION: " << e.what());
        _finished.send(*this);
    }
}


void Acceptor::onReplySent(Reply& r)
{
    PT_LOG_TRACE("Acceptor::onReplySent");

    try
    {
        MessageProgress progress = _reply.endSend();

        if( ! progress.finished() )
        {
            PT_LOG_DEBUG("continue writing reply");
            bool finished = _reply.isFinished();
            _reply.beginSend(finished);
            return;
        }

        if( _reply.isFinished() )
        {
            PT_LOG_DEBUG("reply finished");

            if (r.statusCode() == 101)
            {
                PT_LOG_DEBUG("upgrade");

                Connection* conn = this->release();
                _server.upgrade(conn);
            }

            releaseResponder();

            _reply.clear();
            _request.clear();

            if( _conn == 0 || ! _conn->isConnected() )
            {
                PT_LOG_DEBUG("not connected anymore");
                _finished.send(*this);
                return;
            }

            _request.beginReceive();
            return;
        }

        // reply chunks are written while reading request
        //
        // TODO: is this still possible?
        //
        //  probably need setReady(Input|Output|Done) ?
        //
        //  - read part of request
        //  - write part of reply
        //  - switch back to read next part of request
        //  - switch back to write next part of reply

        // TODO: same as _request->isFinished()
        if( ! _requestProgress.finished() )
        {
            PT_LOG_DEBUG("continuing request");
            _request.beginReceive();
            return;
        }

        PT_LOG_DEBUG("continuing reply");
        _reply.discard();
        
        _state = OnWriteReply;

        assert(_responder);
        _responder->writeReply(_request, _reply, *_conn->loop());

    }
    catch(const System::IOError& e) // TODO: HttpError is also an IOError
    {
        PT_LOG_WARN("EXCEPTION: " << e.what());
        _finished.send(*this);
    }
}


void Acceptor::setFinished(bool isFinished)
{
    if(_state == OnBeginRequest || _state == OnReadRequest)
    {
        if(isFinished)
        {
            _request.discard();
            _reply.beginSend(true);
            return;
        }

        bool isRequestFinished = _request.isFinished();
        if( ! _request.isFinished() )
        {
            _request.beginReceive();
            return;
        }

        _state = OnBeginReply;
        _responder->beginReply( _request, _reply, *_conn->loop() );
        return;
    }

    if(_state == OnBeginReply || _state == OnWriteReply)
    {
        _reply.beginSend(isFinished);
        return;
    }
}


void Acceptor::replyError()
{
    _reply.clear();

    _reply.setStatus(400, "Bad Request");
    _reply.header().set("Content-Type", "text/plain");
    _reply.header().set("Connection", "close");
    _reply.body() << "Error 400: Bad Request.";

    _reply.beginSend(true);
}




ServerThread::ServerThread()
: _ssl(false)
, _thread(_loop)
, _isReturned(false)
, _isServletIdle(false)
{
    _loop.eventReceived() += Pt::slot(*this, &ServerThread::onAccept);
    _loop.eventReceived() += Pt::slot(*this, &ServerThread::onRemoveHandler);
    _loop.eventReceived() += Pt::slot(*this, &ServerThread::onRemoveServlet);
    _loop.eventReceived() += Pt::slot(*this, &ServerThread::onIsServletIdle);
    
    _thread.start();
}


ServerThread::~ServerThread()
{
    stop();
}


void ServerThread::setSecure(Ssl::Context& ctx)
{
    _sslctx.assign(ctx);
    _ssl = true;
}
        

void ServerThread::serve(Acceptor* conn)
{
    AcceptEvent ev(conn);
    _loop.commitEvent(ev);
}


void ServerThread::stop()
{
    _loop.exit();

    if( _thread.isJoinable() )
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
    System::MutexLock lock(_invokeMutex);
    _isReturned = false;

    RemoveServletEvent ev(&servlet);
    _loop.commitEvent(ev);

    while( ! _isReturned)
        _hasReturned.wait(lock);
}


bool ServerThread::isServletIdle(Servlet& servlet)
{
    System::MutexLock lock(_invokeMutex);

    _isServletIdle = false;
    _isReturned = false;

    ServletInfoEvent ev(&servlet);
    _loop.commitEvent(ev);

    while( ! _isReturned)
        _hasReturned.wait(lock);

    return _isServletIdle;
}


void ServerThread::onAccept(const AcceptEvent& ev)
{
    Acceptor* handler = ev.connection();

    _handlers.push_back(handler);
    handler->finished() += Pt::slot(*this, &ServerThread::onHandlerFinished);

    if(_ssl)
        handler->setSecure(_sslctx);

    handler->beginServe(_loop);
}


void ServerThread::onRemoveHandler(const RemoveHandlerEvent& ev)
{
    std::vector<Acceptor*>::iterator it;
    for(it = _handlers.begin(); it != _handlers.end(); ++it)
    {
        if(ev.handler() == *it)
        {
            delete *it;
            _handlers.erase(it);
            break;
        }
    }
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


void ServerThread::onIsServletIdle(const ServletInfoEvent& ev)
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
    RemoveHandlerEvent ev(&handler);
    _loop.commitEvent(ev);
}


ServerImpl::ServerImpl()
: _sslctx(0)
, _useWorker(0)
, _maxThreads(1)
, _timeout(30000)
, _keepAliveTimeout(30000)
, _maxRequestSize( std::numeric_limits<std::size_t>::max() )
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


void ServerImpl::listen(const Pt::Net::Endpoint& addr, const Net::TcpServerOptions& opts)
{
    std::vector<ServerThread*>::iterator thread;
    for(thread = _serverThreads.begin(); thread != _serverThreads.end(); ++thread)
    {
        (*thread)->stop();
        delete *thread;
    }

    _serverThreads.clear();

    _serverSocket.listen(addr, opts);
    _serverSocket.beginAccept();

    for(std::size_t n = 1; n < _maxThreads; ++n)
    {
        ServerThread* st = new ServerThread();

        if(_sslctx)
            st->setSecure(*_sslctx);

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

        PT_LOG_INFO("serving: " << request.url());
        return it->servlet();
    }

    PT_LOG_WARN("not found: " << request.url());
    return 0;
}


void ServerImpl::upgrade(Connection* conn)
{
    UpgradeEvent ev(new IOStream(conn));
    loop()->commitEvent(ev);
}


void ServerImpl::onAccept(Net::TcpServer& server)
{
    PT_LOG_TRACE("Server::onAccept");

    // TODO: we should only pass the TcpSocket to the worker thread so that 
    // an Acceptor can be constructed with an event loop there

    Pt::AutoPtr<Acceptor> handler( new Acceptor(*this, server) );

    PT_LOG_DEBUG("handler timeouts: " << _timeout << ", " << _keepAliveTimeout);
    handler->setTimeout(_timeout);
    handler->setKeepAliveTimeout(_keepAliveTimeout);
    handler->setMaxReadSize(_maxRequestSize);

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
    System::EventLoop* eventLoop = this->loop();
    if( ! eventLoop )
        return;

    RemoveHandlerEvent ev(&h);
    eventLoop->commitEvent(ev);
}


void ServerImpl::onRemoveHandler(const RemoveHandlerEvent& ev)
{
    std::vector<Acceptor*>::iterator it;
    for(it = _handlers.begin(); it != _handlers.end(); ++it)
    {
        if(ev.handler() == *it)
        {
            delete *it;
            _handlers.erase(it);
            break;
        }
    }
}

} // namespace Http

} // namespace Pt
