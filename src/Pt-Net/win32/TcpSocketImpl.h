/*
 * Copyright (C) 2006-2009 by Marc Boris Duerner, Tommi Maekitalo
 *                            Laurentiu-Gheorghe Crisan
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

#ifndef PT_NET_TcpSocketImpl_H
#define PT_NET_TcpSocketImpl_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Selector.h"
#include "AddrInfo.h"
#include "Pt/WinVer.h"
#include "Pt/Net/Api.h"
#include <Pt/Net/TcpSocket.h>

#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace Pt {

namespace System {
    class EventLoop;
}

namespace Net {

class Endpoint;
class TcpServer;
class TcpSocket;

class TcpSocketImpl
{
    public:
        TcpSocketImpl(TcpSocket& socket);

        ~TcpSocketImpl();

        void close();

        void cancel(System::EventLoop& loop);

        void accept(TcpServer& server, const TcpSocketOptions& o);

        void connect(const Endpoint& addrinfo, const TcpSocketOptions&);

        bool beginConnect(System::EventLoop& loop, const Endpoint& addrinfo, const TcpSocketOptions&);

        void endConnect(System::EventLoop& loop);

        bool runConnect(System::EventLoop& loop, bool& isConnected);

        size_t beginRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof);

        size_t read(char* buffer, size_t count, bool& eof);

        size_t endRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof);

        bool runRead(System::EventLoop& loop);

        size_t beginWrite(System::EventLoop& loop, const char* buffer, size_t n);

        size_t endWrite(System::EventLoop& loop, const char* buffer, size_t n);

        bool runWrite(System::EventLoop& loop);

        size_t write(const char* buffer, size_t count);

        void localEndpoint(Endpoint& ep) const;

        void remoteEndpoint(Endpoint& ep) const;

        void setTimeout(std::size_t msecs)
        { _timeout = msecs; }

        std::size_t timeout() const
        { return _timeout; }

    private:
        void connect();
        bool beginConnect();
        void setEventFlags(HANDLE ev, long events);
        bool wait(std::size_t msecs);
        int waitSelect(fd_set* rfds, fd_set* wfds, fd_set* efds, size_t timeout);

    private:
        AddrInfo _addrInfo;
        AddrInfo::const_iterator _addrInfoPtr;
        bool _errorPending;
        SOCKET _fd;
        std::size_t _timeout;
        System::IOHandle _ioh;
        WSABUF _sendBuffer;
        WSABUF _receiveBuffer;
        bool _fdClose;
        long _eventFlags;
};

} // namespace Net

} // namespace Pt

#endif

