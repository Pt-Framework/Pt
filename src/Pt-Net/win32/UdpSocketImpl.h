/*
 * Copyright (C) 2010 by Marc Boris Duerner
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

#ifndef PT_NET_UdpSocketImpl_H
#define PT_NET_UdpSocketImpl_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Selector.h"
#include "Pt/WinVer.h"
#include "Pt/Net/Api.h"
#include "Pt/Net/Endpoint.h"
#include "Pt/Net/UdpSocket.h"
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>

namespace Pt {

namespace Net {

class UdpSocket;

class UdpSocketImpl
{
    public:
        UdpSocketImpl(UdpSocket& socket);

        ~UdpSocketImpl();

        void close();

        bool beginBind(System::EventLoop& loop, const Endpoint& addrinfo, const UdpSocketOptions& o);

        bool runBind(System::EventLoop& loop);

        void endBind(System::EventLoop& loop);

        void bind(const Endpoint& addrinfo, const UdpSocketOptions& o);

        bool beginConnect(System::EventLoop& loop, const Endpoint& addrinfo, const UdpSocketOptions& o);

        bool runConnect(System::EventLoop& loop);

        void endConnect(System::EventLoop& loop);

        void connect(const Endpoint& addrinfo, const UdpSocketOptions& o);

        void setTarget(const Endpoint& addrinfo, const UdpSocketOptions& o);

        bool isConnected() const;

        bool isBound() const;

        void setHopLimit(unsigned int n);

        void joinMulticastGroup(const std::string& ipaddr);

        void dropMulticastGroup(const std::string& ipaddr);

        void cancel(System::EventLoop& loop);

        void localEndpoint(Endpoint& ep) const;

        const Endpoint& remoteEndpoint() const;

        void setTimeout(std::size_t msecs)
        { _timeout = msecs; }

        std::size_t timeout() const
        { return _timeout; }

        size_t beginRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof);

        size_t read(char* buffer, size_t count, bool& eof);

        size_t endRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof);

        size_t beginWrite(System::EventLoop& loop, const char* buffer, size_t n);

        size_t write(const char* buffer, size_t n);

        size_t endWrite(System::EventLoop& loop, const char* buffer, size_t n);

        bool runRead(System::EventLoop& loop);

        bool runWrite(System::EventLoop& loop);

    protected:
        void setEventFlags(HANDLE ev, long events);

        int waitSelect(fd_set* rfds, fd_set* wfds, fd_set* efds, size_t timeout);

    private:
        System::IOHandle _ioh;
        SOCKET           _fd;
        bool             _isConnected;
        bool             _isBound;
        Endpoint         _peerAddr;
        sockaddr_storage _sendAddr;
        mutable sockaddr_storage _servaddr;
        unsigned int     _hopLimit;
        long             _eventFlags;
        std::size_t      _timeout;
        WSABUF           _sendBuffer;
        WSABUF           _receiveBuffer;
};

} // namespace Net

} // namespace Pt

#endif
