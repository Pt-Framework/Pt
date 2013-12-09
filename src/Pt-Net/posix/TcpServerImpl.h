/*
 * Copyright (C) 2006-2009 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef PT_NET_TcpServerImpl_H
#define PT_NET_TcpServerImpl_H

#include <Pt/Net/Api.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include "Selector.h"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

namespace Pt {

namespace System {
    class EventLoop;
    class IOHandle;
}

namespace Net {

class TcpServer;

class TcpServerImpl
{
    private:
        TcpServer& _server;
        System::IOHandle _ioh;
        size_t _timeout;
        int _acceptedFd;
        struct sockaddr_storage _servaddr;

    public:
        TcpServerImpl(TcpServer& server);

        virtual ~TcpServerImpl() 
        {}

        void setTimeout(std::size_t msecs)
        { _timeout = msecs; }

        std::size_t timeout() const
        { return _timeout; }

        void create(int domain, int type, int protocol);

        void close();

        void listen(const std::string& ipaddr,
                    unsigned short int port,
                    const TcpServerOptions& options);

        void listen(const Endpoint& ep, const TcpServerOptions& options);

        void beginAccept(System::EventLoop& loop);

        const struct sockaddr_storage& getAddr() const
        { return _servaddr; }

        int fd() const
        { return _ioh.fd; }

        int accept(const TcpSocketOptions& o);

        void cancel(System::EventLoop& s);
    
        bool run(System::EventLoop& loop);
};

} // namespace Net

} // namespace Pt

#endif
