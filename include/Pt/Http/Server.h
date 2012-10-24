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
#include <Pt/Net/TcpServer.h>
#include <Pt/Connectable.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <cstddef>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Net {
class AddrInfo;
}

namespace Ssl {
class Context;
}

namespace Http {

class Request;
class Servlet;
class ServerImpl;

class PT_HTTP_API Server : public Connectable
                         , private NonCopyable
{
    public:
        class Options
        {
            public:
                Options()
                : _maxThreads(1)
                , _sslctx(0)
                {}

                unsigned maxThreads() const
                { return _maxThreads; }

                void setMaxThreads(unsigned m)
                { _maxThreads = m; }

                void setSecure(Ssl::Context& ctx)
                { _sslctx = &ctx; }

                Ssl::Context* sslContext() const
                { return _sslctx; }

                Net::TcpServer::Options& tcp()
                { return _tcpOptions; }

                const Net::TcpServer::Options& tcp() const
                { return _tcpOptions; }

            private:
                Net::TcpServer::Options _tcpOptions;
                std::size_t _maxThreads;
                Ssl::Context* _sslctx;
        };

    public:
        Server();

        explicit Server(System::EventLoop& loop);

        Server(System::EventLoop& loop, const std::string& ip, unsigned short int port, const Options& = Options());

        Server(System::EventLoop& loop, const Net::AddrInfo& addr, const Options& = Options());

        ~Server();

        System::EventLoop* loop();

        void setActive(System::EventLoop& loop);

        std::size_t timeout() const;

        void setTimeout(std::size_t ms);

        std::size_t keepAliveTimeout() const;

        void setKeepAliveTimeout(std::size_t ms);

        void listen(const std::string& ip, unsigned short int port, const Options& options = Options());

        void listen(const Net::AddrInfo& addr, const Options& options = Options());

        void cancel();

        void addServlet(Servlet& servlet);

        void removeServlet(Servlet& servlet);

        void shutdownServlet(Servlet& servlet, bool shutdown);

        bool isServletIdle(Servlet& servlet);

        Servlet* getServlet(const Request& request);

    private:
        ServerImpl* _impl;
};

} // namespace Http

} // namespace Pt

#endif
