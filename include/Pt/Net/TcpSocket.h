/*
 * Copyright (C) 2006-2013 by Marc Boris Duerner
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

#ifndef Pt_Net_TcpSocket_h
#define Pt_Net_TcpSocket_h

#include <Pt/Net/Api.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/System/IODevice.h>
#include <string>

namespace Pt {

namespace Net {

/** @brief TCP client socket
 */
class PT_NET_API TcpSocket : public System::IODevice
{
    public:
        // TCP_NODELAY
        // SO_KEEPALIVE
        // SO_SNDBUF 
        class Options
        {
            public:
                explicit Options()
                : _flags(0)
                {}

            private:
                unsigned long _flags;
        };

    public:
        TcpSocket();

        TcpSocket(const TcpServer& server, const Options& o = Options());

        /** @brief Connect to a host
            
            @throw System::AccessFailed if the host is not reachable
         */
        TcpSocket(const std::string& ipaddr, unsigned short int port, const Options& o = Options());

        explicit TcpSocket(const Endpoint& addrinfo, const Options& o = Options());

        ~TcpSocket();

        void localEndpoint(Endpoint& ep) const;

        void remoteEndpoint(Endpoint& ep) const;

        void accept(const TcpServer& server, const Options& o = Options());

        void connect(const Endpoint& addrinfo, const Options& o = Options());

        /** @brief Connect to a host
            
            @throw System::AccessFailed if the host is not reachable
         */
        void connect(const std::string& ipaddr, unsigned short int port, const Options& o = Options());

        bool beginConnect(const Endpoint& addrinfo, const Options& o = Options());

        bool beginConnect(const std::string& ipaddr, unsigned short int port, const Options& o = Options());

        void endConnect();

        Signal<TcpSocket&>& connected()
        { return _connected; }

        bool isConnected() const;

    protected:
        // inherit doc
        virtual void onClose();

        virtual void onSetTimeout(size_t timeout);

        // inherit doc
        virtual bool onRun();

        // inherit doc
        virtual size_t onBeginRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof);

        // inherit doc
        virtual size_t onEndRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof);

        // inherit doc
        virtual size_t onRead(char* buffer, size_t count, bool& eof);

        // inherit doc
        virtual size_t onBeginWrite(System::EventLoop& loop, const char* buffer, size_t n);

        // inherit doc
        virtual size_t onEndWrite(System::EventLoop& loop, const char* buffer, size_t n);

        // inherit doc
        virtual size_t onWrite(const char* buffer, size_t count);

        // inherit doc
        virtual void onCancel();

    private:
        //! @internal
        class TcpSocketImpl* _impl;

        //! @internal
        Signal<TcpSocket&> _connected;

        //! @internal
        bool _connecting;

        //! @internal
        bool _reservedForIsConnectedFlag;
};

} // namespace Net

} // namespace Pt

#endif

