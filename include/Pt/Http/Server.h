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

#ifndef Pt_Http_Server_h
#define Pt_Http_Server_h

#include <Pt/Http/Api.h>
#include <Pt/Http/Request.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Connectable.h>
#include <Pt/NonCopyable.h>
#include <vector>
#include <string>
#include <cstddef>
#include <cassert>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Ssl {
class Context;
}

namespace Http {

class ServerThread;
class RequestHandler;
class Reply;
class Servlet;

class PT_HTTP_API Server : public Connectable
                         , private NonCopyable
{
    friend class Servlet;

    public:
        Server();

        explicit Server(System::EventLoop& eventLoop);

        Server(System::EventLoop& eventLoop, const std::string& ip, unsigned short int port, int backlog = 5);

        Server(System::EventLoop& eventLoop, const Pt::Net::AddrInfo& addr, int backlog = 5);

        ~Server();

        System::EventLoop* loop()
        { return _serverSocket.parent(); }

        void setActive(System::EventLoop& loop);

        void setSecure(Ssl::Context& ctx);

        std::size_t timeout() const;

        void setTimeout(std::size_t ms);

        std::size_t keepAliveTimeout() const;

        void setKeepAliveTimeout(std::size_t ms);

        unsigned maxThreads() const;

        void setMaxThreads(unsigned m);

        // TODO: 
        // Pt::Net::SocketOptions options;
        // options.setBacklog(5);
        // options.setFlags(Pt::Net::DeferAccept);
        // server.listen(ip, port, options);
        void listen(const std::string& ip, unsigned short int port, int backlog = 5);

        void listen(const Pt::Net::AddrInfo& addr, int backlog = 5);

        void cancel();

        void addServlet(Servlet& servlet);

        void removeServlet(Servlet& servlet);

        Servlet* getServlet(const Request& request);

    private:
        void startWorker();

        void onAccept(Net::TcpServer& server);

        void onHandlerFinished(RequestHandler& conn);

    private:
        Net::TcpServer _serverSocket;
        Ssl::Context* _sslctx;
        bool _ssl;
        std::vector<ServerThread*> _serverThreads;
        std::vector<RequestHandler*> _handlers;
        unsigned _useWorker;
        unsigned _maxThreads;
        std::size_t _timeout;
        std::size_t _keepAliveTimeout;

        System::ReadWriteMutex _serviceMutex;
        typedef std::vector<Servlet*> ServletList;
        ServletList _servlets;
};

} // namespace Http

} // namespace Pt

#endif

