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

/** @brief UDP socket options.
 */
class PT_NET_API UdpSocketOptions
{
    public:
        /** @brief Default constructor.
        */
        UdpSocketOptions();

        /** @brief Copy constructor.
        */
        UdpSocketOptions(const UdpSocketOptions& opts);

        /** @brief Destructor.
        */
        ~UdpSocketOptions();

        /** @brief Assignment operator.
        */
        UdpSocketOptions& operator=(const UdpSocketOptions& opts);
                
        /** @brief Returns true if UDP broadcast is enabled.
        */
        bool isBroadcast() const
        { return (_flags & Broadcast) != 0; }
        
        /** @brief enables UDP broadcast.
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


/** @brief UDP server and client socket.
 */
class PT_NET_API UdpSocket : public System::IODevice
{
    public:
        /** @brief Default constructor.
        */
        UdpSocket();

        /** @brief Construct with event loop.
        */
        explicit UdpSocket(System::EventLoop& loop);

        /** @brief Destructor.
        */
        ~UdpSocket();
        
        /** @brief Bind to local endpoint.

            @throw System::AccessFailed if the host is not reachable
        */
        void bind(const Endpoint& ep);

        /** @brief Bind to local endpoint.

            @throw System::AccessFailed if the host is not reachable
        */
        void bind(const Endpoint& ep, const UdpSocketOptions& o);

        //void bindMulticast(const Endpoint& e);

        /** @brief Begin bind to local endpoint.

            Begins binding to the Endpoint @a ep. The %UdpSocket must be
            attached to a event loop with setActive(). Once the binding has
            completed, the signal bound() will be sent. In response, the
            method endBind() has to be called to finish the bind operation.

            @throw System::AccessFailed if the host is not reachable
        */
        bool beginBind(const Endpoint& ep);

        /** @brief Begin bind to local endpoint.

            @throw System::AccessFailed if the host is not reachable
        */
        bool beginBind(const Endpoint& ep, const UdpSocketOptions& o);

        //bool beginBindMulticast(const Endpoint& iface, const UdpSocketOptions& o);

        /** @brief end bind to local endpoint.

            @throw System::AccessFailed if the host is not reachable
        */
        void endBind();

        /** @brief Notifies that the socket was bound.
            
            This signal is send when the %UdpSocket is monitored
            in an EventLoop and was bound to a local endpoint.
        */
        Signal<UdpSocket&>& bound()
        { return _bound; }

        /** @brief Returns true if bound.
        */
        bool isBound() const;

        /** @brief Connect to an endpoint.
            
            @throw System::AccessFailed if the host is not reachable
        */
        void connect(const Endpoint& ep);

        /** @brief Connect to an endpoint.
            
            @throw System::AccessFailed if the host is not reachable
        */
        void connect(const Endpoint& ep, const UdpSocketOptions& o);

        /** @brief Set target endpoint.
            
            @throw System::AccessFailed if the host is not reachable
        */
        void setTarget(const Endpoint& ep);

        /** @brief Set target endpoint.
            
            @throw System::AccessFailed if the host is not reachable
        */
        void setTarget(const Endpoint& ep, const UdpSocketOptions& o);

        /** @brief Begin connect to an endpoint.
            
            @throw System::AccessFailed if the host is not reachable
        */
        bool beginConnect(const Endpoint& ep);

        /** @brief Begin connect to an endpoint.
            
            @throw System::AccessFailed if the host is not reachable
        */
        bool beginConnect(const Endpoint& ep, const UdpSocketOptions& o);

        /** @brief End connect to an endpoint.
            
            @throw System::AccessFailed if the host is not reachable
        */
        void endConnect();

        /** @brief Notifies that the socket was connected.
            
            This signal is send when the %UdpSocket is monitored
            in an EventLoop and a connection was established.
        */
        Signal<UdpSocket&>& connected()
        { return _connected; }

        /** @brief Returns true if connected.
        */
        bool isConnected() const;

        /** @brief Joins a multicast group.
        */
        void joinMulticastGroup(const std::string& ipaddr);

        //void dropMulticastGroup(const std::string& ipaddr);

        /** @brief Gets the local endpoint.
        */
        void localEndpoint(Endpoint& ep) const;

        /** @brief Gets the remote endpoint.
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
