/*
 * Copyright (C) 2009 Tommi Maekitalo
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

#ifndef PT_HTTP_SERVERIMPL_H
#define PT_HTTP_SERVERIMPL_H

#include <cstddef>
#include <set>
#include <Pt/System/Queue.h>
#include <Pt/Connectable.h>
#include <Pt/Event.h>
#include <Pt/Http/Server.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/Responder.h>
#include "Socket.h"
#include "NotFoundService.h"
#include "NotAuthenticatedService.h"

namespace Pt
{

class EventLoopBase;

namespace Http
{

class Worker;
class Listener;
class ServerImpl;

class IdleSocketEvent : public BasicEvent<IdleSocketEvent>
{
        Socket* _socket;

    public:
        explicit IdleSocketEvent(Socket* socket)
            : _socket(socket)
            { }

        Socket* socket() const   { return _socket; }

};

class ServerStartEvent : public BasicEvent<ServerStartEvent>
{
        const ServerImpl* _server;

    public:
        explicit ServerStartEvent(const ServerImpl* server)
            : _server(server)
            { }

        const ServerImpl* server() const   { return _server; }

};

class NoWaitingThreadsEvent : public BasicEvent<NoWaitingThreadsEvent>
{
};

class ThreadTerminatedEvent : public BasicEvent<ThreadTerminatedEvent>
{
        Worker* _worker;

    public:
        explicit ThreadTerminatedEvent(Worker* worker)
            : _worker(worker)
            { }

        Worker* worker() const   { return _worker; }
};

class ServerImpl : public Connectable
{
    public:
        ServerImpl(System::EventLoopBase& eventLoop, Signal<Server::Runmode>& runmodeChanged);
        ~ServerImpl();

        void listen(const std::string& ip, unsigned short int port);
        void noWaitingThreads();

        void addService(const std::string& url, Service& service);
        void removeService(Service& service);

        Responder* getResponder(const Request& request);
        Responder* getDefaultResponder(const Request& request)
            { return _defaultService.createResponder(request); }

        std::size_t readTimeout() const       { return _readTimeout; }
        std::size_t writeTimeout() const      { return _writeTimeout; }
        std::size_t keepAliveTimeout() const  { return _keepAliveTimeout; }
        std::size_t idleTimeout() const       { return _idleTimeout; }

        void readTimeout(std::size_t ms)      { _readTimeout = ms; }
        void writeTimeout(std::size_t ms)     { _writeTimeout = ms; }
        void keepAliveTimeout(std::size_t ms) { _keepAliveTimeout = ms; }
        void idleTimeout(std::size_t ms)      { _idleTimeout = ms; }

        unsigned minThreads() const           { return _minThreads; }
        void minThreads(unsigned m)           { _minThreads = m; }

        unsigned maxThreads() const           { return _maxThreads; }
        void maxThreads(unsigned m)           { _maxThreads = m; }

        void onInput(Socket& _socket);
        void onKeepAliveTimeout(Socket& _socket);

        bool isTerminating() const
        { return _runmode == Server::Terminating; }

        void terminate();
        Server::Runmode runmode() const
        { return _runmode; }

    private:
        void runmode(Server::Runmode runmode)
        {
            _runmode = runmode;
            _runmodeChanged(runmode);
        }

        void addIdleSocket(Socket* _socket);
        void onIdleSocket(const IdleSocketEvent& event);
        void onNoWaitingThreads(const NoWaitingThreadsEvent& event);
        void onThreadTerminated(const ThreadTerminatedEvent& event);
        void onServerStart(const ServerStartEvent& event);
        void start();

        friend class Worker;

        ////////////////////////////////////////////////////
        System::EventLoopBase& _eventLoop;

        std::size_t _readTimeout;
        std::size_t _writeTimeout;
        std::size_t _keepAliveTimeout;
        std::size_t _idleTimeout;
        unsigned _minThreads;
        unsigned _maxThreads;
        atomic_t _waitingThreads;

        Signal<Server::Runmode>& _runmodeChanged;
        Server::Runmode _runmode;

        System::Queue<Socket*> _queue;
        std::set<Socket*> _idleSockets;

        ////////////////////////////////////////////////////
        typedef std::vector<Listener*> ListenerType;
        ListenerType _listener;

        ////////////////////////////////////////////////////
        typedef std::set<Worker*> Threads;
        Threads _threads;
        Threads _terminatedThreads;
        System::Mutex _threadMutex;
        System::Condition _threadTerminated;
        void threadTerminated(Worker* worker);

        ////////////////////////////////////////////////////
        typedef std::multimap<std::string, Service*> ServicesType;
        System::Mutex _serviceMutex;
        ServicesType _service;
        NotFoundService _defaultService;
        NotAuthenticatedService _noAuthService;
};

}
}

#endif // PT_HTTP_SERVERIMPL_H

