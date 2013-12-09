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

#include "IODeviceImpl.h"
#include "Pt/Net/Api.h"
#include "Pt/Net/UdpSocket.h"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h> // is this still needed?
#include <sys/time.h>
#include <unistd.h>

namespace Pt {

namespace Net {

class UdpSocket;

class UdpSocketImpl : public System::IODeviceImpl
{
    public:
        UdpSocketImpl(UdpSocket& socket);

        ~UdpSocketImpl();

        void close();

        bool beginBind(System::EventLoop& loop, const Endpoint& ep, const UdpSocketOptions& o);

        bool runBind(System::EventLoop& loop);

        void endBind(System::EventLoop& loop);

        void bind(const Endpoint& ep, const UdpSocketOptions& o);

        bool beginConnect(System::EventLoop& loop, const Endpoint& ep, const UdpSocketOptions& o);

        bool runConnect(System::EventLoop& loop);

        void endConnect(System::EventLoop& loop);

        void connect(const Endpoint& ep, const UdpSocketOptions& o);

        void setTarget(const Endpoint& ep, const UdpSocketOptions& o);

        bool isConnected() const;

        bool isBound() const;

        void setBroadcast();

        void joinMulticastGroup(const std::string& ipaddr);

        void dropMulticastGroup(const std::string& ipaddr);

        void localEndpoint(Endpoint& ep) const;

        const Endpoint& remoteEndpoint() const;

        size_t beginRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof);

        size_t read(char* buffer, size_t count, bool& eof);

        size_t beginWrite(System::EventLoop& loop, const char* buffer, size_t n);

        size_t write(const char* buffer, size_t n);

    private:
        bool             _isConnected;
        bool             _isBound;
        Endpoint         _peerAddr;
        sockaddr_storage _servaddr;
        sockaddr_storage _sendaddr;
        socklen_t        _sendaddrLen;
};

} // namespace Net

} // namespace Pt

#endif
