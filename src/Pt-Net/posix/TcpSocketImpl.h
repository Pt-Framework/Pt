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

#include "Pt/Signal.h"
#include "IODeviceImpl.h"
#include "Pt/Net/AddrInfo.h"
#include "AddrInfoImpl.h"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

namespace Pt {

namespace Net {

class TcpServer;
class TcpSocket;

class TcpSocketImpl : public System::IODeviceImpl
{
    private:
        bool _isConnected;
        bool _isConnecting;
        struct sockaddr_storage _peeraddr;
        AddrInfo _addrInfo;
        AddrInfoImpl::const_iterator _addrInfoPtr;

        int checkConnect();

        void checkPendingError();

        const char* tryConnect();

        const char* _connectResult;

    public:
        TcpSocketImpl(TcpSocket& socket);

        ~TcpSocketImpl();

        void close();

        void cancel(System::EventLoop& loop);

        bool runConnect(System::EventLoop& loop);

        std::string getSockAddr() const;

        std::string getPeerAddr() const;

        bool isConnected() const
        { return _isConnected; }

        void connect(const AddrInfo& addrinfo);

        bool beginConnect(const AddrInfo& addrinfo, System::EventLoop& loop);

        void endConnect(System::EventLoop& loop);

        void accept(const TcpServer& server, unsigned inherit);
};

} // namespace Net

} // namespace Pt

#endif
