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

#ifndef Pt_Http_Server_h
#define Pt_Http_Server_h

#include <Pt/Http/Api.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Signal.h>
#include <Pt/NonCopyable.h>
#include <vector>
#include <string>
#include <cstddef>

namespace Pt {

namespace System {

class EventLoop;

}

namespace Ssl {

class Context;

}

namespace Http {

class ServerThread;
class Service;
class RequestHandler;
class RequestHeader;
class Responder;
class NotFoundService;
class NotAuthenticatedService;

// TODO: It might make sense for the Server to derive from Selectable

class PT_HTTP_API Server : public Pt::Connectable
                         , private Pt::NonCopyable
{
    friend class Service;

    public:
        Server();

        explicit Server(System::EventLoop& eventLoop);

        Server(System::EventLoop& eventLoop, const std::string& ip, unsigned short int port, int backlog = 5);

        Server(System::EventLoop& eventLoop, const Pt::Net::AddrInfo& addr, int backlog = 5);

        ~Server();

        System::EventLoop* loop()
        { return _loop; }

        void setActive(System::EventLoop& loop);

        void listen(const std::string& ip, unsigned short int port, int backlog = 5);

        void listen(const Pt::Net::AddrInfo& addr, int backlog = 5);

        void shutdown();

        void addService(const std::string& url, Service& service);

        void removeService(Service& service);

        void setSecure(Ssl::Context& ctx);

        std::size_t readTimeout() const;

        void setReadTimeout(std::size_t ms);

        std::size_t writeTimeout() const;
        
        void setWriteTimeout(std::size_t ms);

        std::size_t keepAliveTimeout() const;

        void setKeepAliveTimeout(std::size_t ms);

        unsigned maxThreads() const;

        void setMaxThreads(unsigned m);

        Service* findService(const RequestHeader& request);

        Responder* getResponder(const RequestHeader& request);

        Responder* notFoundResponder(const RequestHeader& request);

    protected:
        void startWorker();

        void onAccept(Net::TcpServer& server);

        // TODO: rename onHandlerFinished
        void onConnectionTimeout(RequestHandler& conn);

    private:
        System::EventLoop* _loop;
        Net::TcpServer _serverSocket;
        Ssl::Context* _sslctx;
        bool _ssl;
        std::vector<ServerThread*> _serverThreads;
        std::vector<RequestHandler*> _handlers;
        unsigned _useWorker;
        unsigned _maxThreads;
        std::size_t _readTimeout;
        std::size_t _writeTimeout;
        std::size_t _keepAliveTimeout;

        typedef std::multimap<std::string, Service*> ServiceMap;
        System::ReadWriteMutex _serviceMutex;
        ServiceMap _services;
        NotFoundService* _notFoundService;
        NotAuthenticatedService* _noAuthService;
};

} // namespace Http

} // namespace Pt

#endif

