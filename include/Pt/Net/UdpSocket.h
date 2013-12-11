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
        UdpSocketOptions();

        UdpSocketOptions(const UdpSocketOptions& opts);

        ~UdpSocketOptions();

        UdpSocketOptions& operator=(const UdpSocketOptions& opts);
                
        bool isBroadcast() const
        { return (_flags & Broadcast) != 0; }
                
        void setBroadcast()
        { _flags |= Broadcast; }

        int hopLimit() const
        { return _hoplimit; }

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
        UdpSocket();

        explicit UdpSocket(System::EventLoop& loop);

        ~UdpSocket();

        void bind(const Endpoint& ep);

        void bind(const Endpoint& ep, const UdpSocketOptions& o);

        bool beginBind(const Endpoint& ep);

        bool beginBind(const Endpoint& ep, const UdpSocketOptions& o);

        void endBind();

        Signal<UdpSocket&>& bound()
        { return _bound; }

        bool isBound() const;

        void connect(const Endpoint& ep);

        void connect(const Endpoint& ep, const UdpSocketOptions& o);

        void setTarget(const Endpoint& ep);

        void setTarget(const Endpoint& ep, const UdpSocketOptions& o);

        bool beginConnect(const Endpoint& ep);

        bool beginConnect(const Endpoint& ep, const UdpSocketOptions& o);

        void endConnect();

        Signal<UdpSocket&>& connected()
        { return _connected; }

        bool isConnected() const;

        void joinMulticastGroup(const std::string& ipaddr);

        //void dropMulticastGroup(const std::string& ipaddr);

        void localEndpoint(Endpoint& ep) const;

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
