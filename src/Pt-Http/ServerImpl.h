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

#ifndef Pt_Http_ServerImpl_h
#define Pt_Http_ServerImpl_h

#include "Connection.h"

#include <Pt/Http/Api.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Server.h>
#include <Pt/Ssl/Context.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Condition.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>

#include <vector>
#include <string>
#include <cstddef>
#include <cassert>

namespace Pt {

namespace Http {

class Authorization;
class Responder;
class Servlet;
class ServerImpl;
class ServerThread;

class Acceptor : public Pt::Connectable
{
    public:
        Acceptor(ServerImpl& server, Net::TcpServer& tcpServer);

        ~Acceptor();

        void setSecure(Ssl::Context& ctx)
        { _conn.setSecure(ctx); }

        void setTimeout(std::size_t timeout)
        { _conn.setTimeout(timeout); }

        void setKeepAliveTimeout(std::size_t timeout)
        { _conn.setKeepAliveTimeout(timeout); }

        void setMaxReadSize(std::size_t maxSize)
        { _conn.setMaxReadSize(maxSize); }

        void beginServe(System::EventLoop& loop);

        Signal<Acceptor&>& finished()
        { return _finished; }

        Servlet* servlet()
        { return _servlet; }

    protected:
        void releaseResponder();

        void onRequestReceived(Request& req);

        void onAuthorization(Authorization& auth);

        void onRequest(MessageProgress progress);

        void onReplySent(Reply& r);

        void replyError();

    private:
        ServerImpl& _server;
        Authorization* _auth;
        Servlet* _servlet;
        Responder* _responder;
        Connection _conn;
        Request _request;
        Reply _reply;
        MessageProgress _requestProgress;
        Signal<Acceptor&> _finished;
};

class ServerThread : public Connectable 
{
    public:
        class AcceptEvent : public Pt::BasicEvent<AcceptEvent>
        {
            public:
                AcceptEvent(Acceptor* conn)
                : _conn(conn)
                { }
    
                Acceptor* connection() const
                { return _conn; }
    
            private:
                Acceptor* _conn;
        };

        class RemoveServletEvent : public Pt::BasicEvent<RemoveServletEvent>
        {
            public:
                RemoveServletEvent(Servlet* s)
                : _servlet(s)
                { }
    
                Servlet* servlet() const
                { return _servlet; }
    
            private:
                Servlet* _servlet;
        };

        class ServletInfoEvent : public Pt::BasicEvent<ServletInfoEvent>
        {
            public:
                ServletInfoEvent(Servlet* s)
                : _servlet(s)
                { }
    
                Servlet* servlet() const
                { return _servlet; }
    
            private:
                Servlet* _servlet;
        };

    public:
        ServerThread();

        ~ServerThread();

        void setSecure(Ssl::Context& ctx);
        
        void serve(Acceptor* conn);

        void stop();

        void removeServlet(Servlet& servlet);

        bool isServletIdle(Servlet& servlet);

    private:
        void onAccept(const AcceptEvent& ev);

        void onRemoveServlet(const RemoveServletEvent& ev);

        void onIsServletIdle(const ServletInfoEvent& ev);

        void onHandlerFinished(Acceptor& handler);

    private:
        Pt::System::MainLoop _loop;
        
        bool _ssl;
        Ssl::Context _sslctx;

        Pt::System::AttachedThread _thread;
        std::vector<Acceptor*> _handlers;

        bool _isReturned;
        bool _isServletIdle;
        System::Mutex _invokeMutex;
        System::Condition _hasReturned;
};

class ServerImpl : public Connectable
                 , private NonCopyable
{
    public:
        ServerImpl();

        ~ServerImpl();

        System::EventLoop* loop()
        { return _serverSocket.loop(); }

        void setActive(System::EventLoop& eventLoop)
        { _serverSocket.setActive(eventLoop); }

        std::size_t timeout() const
        { return _timeout; }

        void setTimeout(std::size_t ms)
        { _timeout = ms; }

        void setSecure(Ssl::Context& ctx)
        { _sslctx = &ctx; }

        std::size_t maxThreads() const
        { return _maxThreads; }

        void setMaxThreads(std::size_t m)
        { _maxThreads = m; }

        std::size_t keepAliveTimeout() const
        { return _keepAliveTimeout; }

        void setKeepAliveTimeout(std::size_t ms)
        { _keepAliveTimeout = ms; }

        std::size_t maxRequestSize() const
        { return _maxRequestSize; }

        void setMaxRequestSize(std::size_t maxSize)
        { _maxRequestSize = maxSize; }

        void listen(const Pt::Net::Endpoint& addr, const Net::TcpServerOptions& opts);

        void cancel();

        void addServlet(Servlet& servlet);

        void removeServlet(Servlet& servlet);

        void shutdownServlet(Servlet& servlet, bool shutdown);

        bool isServletIdle(Servlet& servlet);

        Servlet* getServlet(const Request& request);

    private:
        void onAccept(Net::TcpServer& server);

        void onHandlerFinished(Acceptor& conn);

    private:
        struct ServletListEntry
        {
            explicit ServletListEntry(Servlet* srv)
            : _servlet(srv)
            , _shutdown(false)
            {}

            Servlet* servlet()
            { return _servlet; }

            bool isShutdown() const
            { return _shutdown; }
            
            void setShutdown(bool shutdown)
            { _shutdown = shutdown; }

            Servlet* _servlet;
            bool _shutdown;
        };

        Net::TcpServer _serverSocket;
        Ssl::Context* _sslctx;
        std::vector<ServerThread*> _serverThreads;
        std::vector<Acceptor*> _handlers;
        std::size_t _useWorker;
        std::size_t _maxThreads;
        std::size_t _timeout;
        std::size_t _keepAliveTimeout;
        std::size_t _maxRequestSize;
        System::ReadWriteMutex _serviceMutex;
        typedef std::vector<ServletListEntry> ServletList;
        ServletList _servlets;
};

} // namespace Http

} // namespace Pt

#endif

