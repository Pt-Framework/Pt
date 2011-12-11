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

#include <Pt/Http2/Api.h>
#include <Pt/Signal.h>
#include <Pt/NonCopyable.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <vector>
#include <string>
#include <cstddef>

namespace Pt {

namespace System {

class EventLoop;

}

namespace Http {

class Request;
class Service;
class TcpConnection;
class Responder;
class NotFoundService;
class NotAuthenticatedService;

class ServerThread;

class PT_HTTP_API Server : public Pt::Connectable
                         , private Pt::NonCopyable
{
    public:
        explicit Server(System::EventLoop& eventLoop);

        Server(System::EventLoop& eventLoop, const std::string& ip, unsigned short int port, int backlog = 5);

        ~Server();

        void listen(const std::string& ip, unsigned short int port, int backlog = 5);

        void addService(const std::string& url, Service& service);

        void removeService(Service& service);

        Responder* getResponder(const Request& request);

        Responder* getDefaultResponder(const Request& request);

        std::size_t readTimeout() const;

        std::size_t writeTimeout() const;

        std::size_t keepAliveTimeout() const;

        void readTimeout(std::size_t ms);

        void writeTimeout(std::size_t ms);

        void keepAliveTimeout(std::size_t ms);

        unsigned minThreads() const;

        void minThreads(unsigned m);

        unsigned maxThreads() const;

        void maxThreads(unsigned m);

        enum Runmode {
          Stopped,
          Starting,
          Running,
          Terminating,
          Failed
        };

        Signal<Runmode> runmodeChanged;

        System::EventLoop& loop()
        {return _loop; }

    protected:
        void startWorker();

        void onAccept(Net::TcpServer& server);

        void onConnectionTimeout(TcpConnection& conn);

    private:
        System::EventLoop& _loop;
        Net::TcpServer _serverSocket;
        std::vector<TcpConnection*> _connections;
        std::vector<ServerThread*> _serverThreads;
        unsigned _useWorker;
        unsigned _minThreads;
        unsigned _maxThreads;
        std::size_t _readTimeout;
        std::size_t _writeTimeout;
        std::size_t _keepAliveTimeout;

        typedef std::multimap<std::string, Service*> ServicesType;
        System::ReadWriteMutex _serviceMutex;
        ServicesType _services;
        NotFoundService* _defaultService;
        NotAuthenticatedService* _noAuthService;
};

} // namespace Http

} // namespace Pt

#endif

