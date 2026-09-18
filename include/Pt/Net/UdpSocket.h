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

#ifndef Pt_Net_UdpSocket_h
#define Pt_Net_UdpSocket_h

#include <Pt/Net/Api.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/System/IODevice.h>
#include <Pt/Types.h>
#include <cstddef>

namespace Pt {

namespace Net {

/** @brief UDP socket bind and send options.

    @ingroup Pt-Net-Udp
*/
class PT_NET_API UdpSocketOptions
{
    public:
        /** @brief Creates default options.
        */
        UdpSocketOptions();

        /** @brief Copies the options.
        */
        UdpSocketOptions(const UdpSocketOptions& opts);

        /** @brief Destroys the options.
        */
        ~UdpSocketOptions();

        /** @brief Assigns the options.
        */
        UdpSocketOptions& operator=(const UdpSocketOptions& opts);
                
        /** @brief Returns true if UDP broadcast is enabled.
        */
        bool isBroadcast() const
        { return (_flags & Broadcast) != 0; }
        
        /** @brief Enables UDP broadcast.
        */
        void setBroadcast()
        { _flags |= Broadcast; }

        /** @brief Returns the hop limit.
        */
        int hopLimit() const
        { return _hoplimit; }

        /** @brief Sets the hop limit.
        */
        void setHopLimit(int n)
        { _hoplimit = n; }

    private:
        //! @internal
        enum Flags
        { 
            Broadcast = 1
        };

        Pt::uint32_t _flags;
        int _hoplimit;
        varint_t _r0;
        varint_t _r1;
        varint_t _r2;
};


/** @brief UDP datagram socket.

    %UdpSocket is the datagram %IODevice for unicast, broadcast, and
    multicast, so one type covers all three modes. After a local bind
    or a send destination is set, %read() and %write() are the inherited
    device operations, blocking or asynchronous, except that each
    %write() sends one datagram and each %read() receives one datagram.

    %bind() sets the local endpoint that receives datagrams, throwing
    %AddressInUse if that address is already occupied and %AccessFailed
    if the host cannot be used. %connect() associates a remote peer so
    writes go there and reads come from it, while %setTarget() sets a
    send destination without associating the socket, which is the path
    broadcast and multicast sends use. %connect() and %setTarget()
    throw %AccessFailed when the host is not reachable.

    @par Asynchronous bind and connect

    %beginBind() and %beginConnect() start those operations on an
    attached event loop and return true when the operation completed
    immediately. %bound() and %connected() are emitted when the attempt
    finishes, and %endBind() and %endConnect() complete it. The socket
    must be attached before either begin method is called. %isBound()
    and %isConnected() report the current associations, %localEndpoint()
    writes the local side, and %remoteEndpoint() is the associated peer.

    @par Broadcast and multicast

    Broadcast send sets the broadcast option and uses the IPv4
    broadcast endpoint as the target, while multicast send uses a
    multicast group address as the target. To receive a group's
    datagrams, bind first, then %joinMulticastGroup(). %UdpSocketOptions
    also set the hop limit; those values configure an operation, but
    they do not open a socket.

    The first example is asynchronous unicast: one socket binds the
    local any-address, the other connects to loopback, and each slot
    completes the begin operation before I/O. The second example is a
    broadcast send, and the third binds, joins a group, and sends to
    that group.

    @code
    char buffer[256];

    void onBound(Pt::Net::UdpSocket& socket)
    {
        socket.endBind();
        socket.beginRead(buffer, sizeof(buffer));
    }

    void onConnected(Pt::Net::UdpSocket& socket)
    {
        socket.endConnect();
        socket.beginWrite("Hello", 5);
    }

    Pt::System::MainLoop loop;

    Pt::Net::UdpSocket receiver;
    receiver.setActive(loop);
    receiver.bound() += Pt::slot(onBound);
    receiver.beginBind(Pt::Net::Endpoint::ip4Any(8000));

    Pt::Net::UdpSocket sender;
    sender.setActive(loop);
    sender.connected() += Pt::slot(onConnected);
    sender.beginConnect(Pt::Net::Endpoint::ip4Loopback(8000));

    loop.run();
    @endcode

    @code
    Pt::Net::UdpSocketOptions opts;
    opts.setBroadcast();

    Pt::Net::UdpSocket sender;
    sender.setTarget(Pt::Net::Endpoint::ip4Broadcast(8000), opts);
    sender.write("Hello", 5);
    @endcode

    @code
    Pt::Net::UdpSocket receiver;
    receiver.bind(Pt::Net::Endpoint::ip4Any(8000));
    receiver.joinMulticastGroup("224.0.1.1");

    Pt::Net::UdpSocket sender;
    sender.setTarget(Pt::Net::Endpoint("224.0.1.1", 8000));
    sender.write("Hello", 5);
    @endcode

    @ingroup Pt-Net-Udp
*/
class PT_NET_API UdpSocket : public System::IODevice
{
    public:
        /** @brief Creates a closed socket.
        */
        UdpSocket();

        /** @brief Creates a socket and attaches it to @a loop.
        */
        explicit UdpSocket(System::EventLoop& loop);

        /** @brief Destroys the socket.
        */
        ~UdpSocket();
        
        /** @brief Binds to local endpoint @a ep.

            @throw %AddressInUse if the local address is already occupied.
            @throw %System::AccessFailed if the host is not reachable.
        */
        void bind(const Endpoint& ep);

        /** @brief Binds to @a ep with @a o.

            @throw %AddressInUse if the local address is already occupied.
            @throw %System::AccessFailed if the host is not reachable.
        */
        void bind(const Endpoint& ep, const UdpSocketOptions& o);

        //void bindMulticast(const Endpoint& e);

        /** @brief Begins binding to @a ep.

            The socket must be attached to an event loop.

            @return true if the bind completed immediately
            @throw %AddressInUse if the local address is already occupied.
            @throw %System::AccessFailed if the host is not reachable.
        */
        bool beginBind(const Endpoint& ep);

        /** @brief Begins binding to @a ep with @a o.

            The socket must be attached to an event loop.

            @return true if the bind completed immediately
            @throw %AddressInUse if the local address is already occupied.
            @throw %System::AccessFailed if the host is not reachable.
        */
        bool beginBind(const Endpoint& ep, const UdpSocketOptions& o);

        //bool beginBindMulticast(const Endpoint& iface, const UdpSocketOptions& o);

        /** @brief Ends binding to a local endpoint.

            @throw %AddressInUse if the local address is already occupied.
            @throw %System::AccessFailed if the host is not reachable.
        */
        void endBind();

        /** @brief Returns the signal that the socket was bound.
        */
        Signal<UdpSocket&>& bound()
        { return _bound; }

        /** @brief Returns true if the socket is bound.
        */
        bool isBound() const;

        /** @brief Connects to @a ep.

            @throw %System::AccessFailed if the host is not reachable.
        */
        void connect(const Endpoint& ep);

        /** @brief Connects to @a ep with @a o.

            @throw %System::AccessFailed if the host is not reachable.
        */
        void connect(const Endpoint& ep, const UdpSocketOptions& o);

        /** @brief Sets the send target to @a ep.

            @throw %System::AccessFailed if the host is not reachable.
        */
        void setTarget(const Endpoint& ep);

        /** @brief Sets the send target to @a ep with @a o.

            @throw %System::AccessFailed if the host is not reachable.
        */
        void setTarget(const Endpoint& ep, const UdpSocketOptions& o);

        /** @brief Begins connecting to @a ep.

            The socket must be attached to an event loop.

            @return true if the connect completed immediately
            @throw %System::AccessFailed if the host is not reachable.
        */
        bool beginConnect(const Endpoint& ep);

        /** @brief Begins connecting to @a ep with @a o.

            The socket must be attached to an event loop.

            @return true if the connect completed immediately
            @throw %System::AccessFailed if the host is not reachable.
        */
        bool beginConnect(const Endpoint& ep, const UdpSocketOptions& o);

        /** @brief Ends connecting to an endpoint.

            @throw %System::AccessFailed if the host is not reachable.
        */
        void endConnect();

        /** @brief Returns the signal that the socket was connected.
        */
        Signal<UdpSocket&>& connected()
        { return _connected; }

        /** @brief Returns true if the socket is connected.
        */
        bool isConnected() const;

        /** @brief Joins multicast group @a ipaddr.
        */
        void joinMulticastGroup(const std::string& ipaddr);

        //void dropMulticastGroup(const std::string& ipaddr);

        /** @brief Writes the local endpoint into @a ep.
        */
        void localEndpoint(Endpoint& ep) const;

        /** @brief Returns the remote endpoint.
        */
        const Endpoint& remoteEndpoint() const;

    protected:
        // inherit doc
        virtual void onClose();

        // inherit doc
        void onSetTimeout(std::size_t timeout);

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
        class UdpSocketImpl* _impl;

        //! @internal
        Signal<UdpSocket&> _connected;

        //! @internal
        Signal<UdpSocket&> _bound;

        //! @internal
        bool _connecting;

        //! @internal
        bool _binding;
};

} // namespace Net

} // namespace Pt

#endif
