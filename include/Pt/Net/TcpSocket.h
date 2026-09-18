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

/** @brief TCP socket connection options.

    @ingroup Pt-Net-Tcp
*/
class PT_NET_API TcpSocketOptions
{
    public:
        /** @brief Creates default options.
        */
        TcpSocketOptions();

        /** @brief Copies the options.
        */
        TcpSocketOptions(const TcpSocketOptions& opts);

        /** @brief Destroys the options.
        */
        ~TcpSocketOptions();

        /** @brief Assigns the options.
        */
        TcpSocketOptions& operator=(const TcpSocketOptions& opts);

        /** @brief Returns the keep-alive interval in seconds, or a negative value if unset.
        */
        int keepAlive() const
        { return _keepAlive.i; }

        /** @brief Sets the keep-alive interval in seconds.

            A negative value leaves keep-alive unset.
        */
        void setKeepAlive(int n)
        { _keepAlive.i = n; }

    private:
        Pt::uint32_t _flags;
        int          _sndbufSize;
        varint_t     _keepAlive;
        varint_t     _r1;
        varint_t     _r2;
};


/** @brief Connected TCP byte stream.

    %TcpSocket is the connected TCP %IODevice. The same type is the
    client side of a %connect() and the accepted side of a pending
    %TcpServer connection. After the stream is up, %read() and %write()
    are the inherited device operations, blocking or asynchronous.

    %connect() reaches a remote endpoint and makes this socket the
    client stream. If the host is not reachable, the operation throws
    %AccessFailed. %accept() takes a pending connection from a listening
    server and makes this socket the accepted stream. A socket is one
    side or the other, not both at once: %connect() and %accept() close
    any previous connection first.

    @par Asynchronous connect

    %beginConnect() starts an asynchronous connect. The socket must be
    attached to an event loop. %connected() is emitted when the attempt
    finishes. %endConnect() completes it and throws %AccessFailed if
    the host is not reachable. %isConnected() reports whether the
    stream is up. %localEndpoint() and %remoteEndpoint() write the two
    sides of the connection.

    %TcpSocketOptions configure the connection. They do not open a
    socket. Keep-alive, when set to a non-negative interval in seconds,
    enables periodic probes on the stream. A negative value leaves
    keep-alive unset.

    The example is an asynchronous client connect. The slot calls
    %endConnect() and then writes through the inherited device
    operation. The accept path is the %TcpServer pending-connection
    slot.

    @code
    void onConnected(Pt::Net::TcpSocket& socket)
    {
        socket.endConnect();
        socket.beginWrite("Hello", 5);
    }

    Pt::System::MainLoop loop;
    Pt::Net::TcpSocket socket;
    socket.setActive(loop);
    socket.connected() += Pt::slot(onConnected);
    socket.beginConnect(Pt::Net::Endpoint("127.0.0.1", 9000));
    loop.run();
    @endcode

    @ingroup Pt-Net-Tcp
*/
class PT_NET_API TcpSocket : public System::IODevice
{
    public:
        /** @brief Creates a closed socket.
        */
        TcpSocket();

        /** @brief Creates a socket and attaches it to @a loop.
        */
        explicit TcpSocket(System::EventLoop& loop);

        /** @brief Creates a socket and accepts a connection from @a server.
        */
        explicit TcpSocket(TcpServer& server);

        /** @brief Creates a socket and connects to @a ep.

            @throw %System::AccessFailed if the host is not reachable.
        */
        explicit TcpSocket(const Endpoint& ep);

        /** @brief Destroys the socket.
        */
        ~TcpSocket();

        /** @brief Accepts a connection from @a server.
        */
        void accept(TcpServer& server);

        /** @brief Accepts a connection from @a server with @a o.
        */
        void accept(TcpServer& server, const TcpSocketOptions& o);

        /** @brief Connects to @a ep.

            @throw %System::AccessFailed if the host is not reachable.
        */
        void connect(const Endpoint& ep);
        
        /** @brief Connects to @a ep with @a o.

            @throw %System::AccessFailed if the host is not reachable.
        */
        void connect(const Endpoint& ep, const TcpSocketOptions& o);

        /** @brief Begins connecting to @a ep.

            The socket must be attached to an event loop.

            @throw %System::AccessFailed if the host is not reachable.
        */
        void beginConnect(const Endpoint& ep);

        /** @brief Begins connecting to @a ep with @a o.

            The socket must be attached to an event loop.

            @throw %System::AccessFailed if the host is not reachable.
        */
        void beginConnect(const Endpoint& ep, const TcpSocketOptions& o);

        /** @brief Ends connecting to a host.

            @throw %System::AccessFailed if the host is not reachable.
        */
        void endConnect();

        /** @brief Returns the signal that the socket was connected.
        */
        Signal<TcpSocket&>& connected()
        { return _connected; }

        /** @brief Returns true if the socket is connected.
        */
        bool isConnected() const
        { return _isConnected; }

        /** @brief Writes the local endpoint into @a ep.
        */
        void localEndpoint(Endpoint& ep) const;

        /** @brief Writes the remote endpoint into @a ep.
        */
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
