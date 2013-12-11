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
#include <Pt/Types.h>
#include <cstddef>

namespace Pt {

namespace Net {

// TCP_NODELAY
// SO_KEEPALIVE
// SO_SNDBUF 

/** @brief TCP socket options.
 */
class PT_NET_API TcpSocketOptions
{
    public:
        TcpSocketOptions();

        TcpSocketOptions(const TcpSocketOptions& opts);

        ~TcpSocketOptions();

        TcpSocketOptions& operator=(const TcpSocketOptions& opts);

    private:
        Pt::uint32_t _flags;
        int _sndbufSize;
        varint_t _r0;
        varint_t _r1;
        varint_t _r2;
};


/** @brief TCP client socket.
 */
class PT_NET_API TcpSocket : public System::IODevice
{
    public:
        //! @brief Default Constructor.
        TcpSocket();

        //! @brief Construct with event loop.
        explicit TcpSocket(System::EventLoop& loop);

        //! @brief Accepts a connection.
        explicit TcpSocket(TcpServer& server);

        /** @brief Connects to a host.
            
            @throw System::AccessFailed if the host is not reachable
         */
        explicit TcpSocket(const Endpoint& ep);

        //! @brief Destructor.
        ~TcpSocket();

        //! @brief Accepts a connection.
        void accept(TcpServer& server);

        //! @brief Accepts a connection.
        void accept(TcpServer& server, const TcpSocketOptions& o);

        /** @brief Connect to a host.
            
            @throw System::AccessFailed if the host is not reachable
         */
        void connect(const Endpoint& ep);
        
        void connect(const Endpoint& ep, const TcpSocketOptions& o);

        void beginConnect(const Endpoint& ep);

        void beginConnect(const Endpoint& ep, const TcpSocketOptions& o);

        void endConnect();

        Signal<TcpSocket&>& connected()
        { return _connected; }

        bool isConnected() const
        { return _isConnected; }

        void localEndpoint(Endpoint& ep) const;

        void remoteEndpoint(Endpoint& ep) const;

    protected:
        // inherit doc
        virtual void onClose();

        // inherit doc
        virtual void onSetTimeout(std::size_t timeout);

        // inherit doc
        virtual bool onRun();

        // inherit doc
        virtual std::size_t onBeginRead(System::EventLoop& loop, char* buffer, std::size_t n, bool& eof);

        // inherit doc
        virtual std::size_t onEndRead(System::EventLoop& loop, char* buffer, std::size_t n, bool& eof);

        // inherit doc
        virtual std::size_t onRead(char* buffer, std::size_t count, bool& eof);

        // inherit doc
        virtual std::size_t onBeginWrite(System::EventLoop& loop, const char* buffer, std::size_t n);

        // inherit doc
        virtual std::size_t onEndWrite(System::EventLoop& loop, const char* buffer, std::size_t n);

        // inherit doc
        virtual std::size_t onWrite(const char* buffer, std::size_t count);

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
        bool _isConnected;
};

} // namespace Net

} // namespace Pt

#endif // Pt_Net_TcpSocket_h
