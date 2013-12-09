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

#ifndef PT_NET_TcpSocketImpl_H
#define PT_NET_TcpSocketImpl_H

#include "AddrInfo.h"
#include "IODeviceImpl.h"
#include "Pt/Net/TcpSocket.h"
#include <string>

namespace Pt {

namespace Net {

class TcpServer;
class TcpSocket;

class TcpSocketImpl : public System::IODeviceImpl
{
    public:
        TcpSocketImpl(TcpSocket& socket);

        ~TcpSocketImpl();

        void close();

        void cancel(System::EventLoop& loop);

        bool runConnect(System::EventLoop& loop, bool& isConnected);

        void localEndpoint(Endpoint& ep) const;

        void remoteEndpoint(Endpoint& ep) const;

        void connect(const Endpoint& ep, const TcpSocketOptions&);

        bool beginConnect(System::EventLoop& loop, const Endpoint& ep, const TcpSocketOptions&);

        void endConnect(System::EventLoop& loop);

        void accept(TcpServer& server, const TcpSocketOptions& o);

    protected:
        void connect();
        
        bool beginConnect(System::EventLoop& loop);

    private:
        TcpSocket& _socket;
        bool _errorPending;
        AddrInfo _addrInfo;
        AddrInfo::const_iterator _addrInfoPtr;
};

} // namespace Net

} // namespace Pt

#endif
