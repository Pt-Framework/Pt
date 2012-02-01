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

#ifndef Pt_Net_UdpSocket_h
#define Pt_Net_UdpSocket_h

#include <Pt/Net/Api.h>
#include <Pt/Net/AddrInfo.h>
#include <Pt/System/IODevice.h>

namespace Pt {

namespace Net {

/** @brief UDP server and client socket
 */
class PT_NET_API UdpSocket : public System::IODevice
{
    public:
        enum SocketFlags 
        { 
            None = 0,
            ALL_SOCKET_FLAGS = 0xffffffff
        };

    public:
        UdpSocket();

        ~UdpSocket();

        void bind(const std::string& ipaddr, unsigned short int port, unsigned flags = 0);

        void connect(const std::string& ipaddr, unsigned short int port, unsigned flags = 0);

        void connect(const AddrInfo& addrinfo, unsigned flags = 0);

        bool isConnected() const;

        bool isBound() const;

        void setBroadcast();

        void joinMulticastGroup(const std::string& ipaddr);

        void dropMulticastGroup(const std::string& ipaddr);

        std::string socketAddress() const;

        std::string peerAddress() const;

        void setTimeout(std::size_t msecs);

        std::size_t timeout() const;

    protected:
        // inherit doc
        virtual void onClose();

        virtual bool onRun();

        // inherit doc
        virtual size_t onBeginRead(char* buffer, size_t n, bool& eof);

        // inherit doc
        virtual size_t onEndRead(char* buffer, size_t n, bool& eof);

        // inherit doc
        virtual size_t onRead(char* buffer, size_t count, bool& eof);

        // inherit doc
        virtual size_t onBeginWrite(const char* buffer, size_t n);

        // inherit doc
        virtual size_t onEndWrite(const char* buffer, size_t n);

        // inherit doc
        virtual size_t onWrite(const char* buffer, size_t count);

        // inherit doc
        virtual void onCancel();

    private:
        //! @internal
        class UdpSocketImpl* _impl;
};

} // namespace Net

} // namespace Pt

#endif
