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
#include <Pt/Signal.h>
#include <Pt/System/IODevice.h>

namespace Pt {

namespace Net {

class AddrInfo;

class PT_NET_API UdpSocket : public System::IODevice
{
    class UdpSocketImpl* _impl;

    public:
        UdpSocket();

        ~UdpSocket();

        void bind(const std::string& ipaddr, unsigned short int port, unsigned flags);

        void connect(const std::string& ipaddr, unsigned short int port)
        {
            connect( AddrInfo(ipaddr, port) );
        }

        void connect(const AddrInfo& addrinfo);

        bool isConnected() const;

        bool isBound() const;

        void setBroadcast();

        void joinMulticastGroup(const std::string& ipaddr);

        void dropMulticastGroup(const std::string& ipaddr);

        std::string getSockAddr() const;

        std::string getPeerAddr() const;

        void setTimeout(std::size_t msecs);

        std::size_t timeout() const;

    protected:
        // inherit doc
        virtual void onClose();

        // inherit doc
        virtual bool onWait(std::size_t msecs);

        // inherit doc
        virtual void onAttach(System::EventLoop&);

        // inherit doc
        virtual void onDetach(System::EventLoop&);

        // inherit doc
        virtual size_t onBeginRead(char* buffer, size_t n, bool& eof);

        // inherit doc
        virtual size_t onEndRead(bool& eof);

        // inherit doc
        virtual size_t onRead(char* buffer, size_t count, bool& eof);

        // inherit doc
        virtual size_t onBeginWrite(const char* buffer, size_t n);

        // inherit doc
        virtual size_t onEndWrite();

        // inherit doc
        virtual size_t onWrite(const char* buffer, size_t count);

        // inherit doc
        virtual void onCancel();

    public:
        // inherit doc
        virtual System::SelectableImpl& simpl();

        // inherit doc
        virtual System::IODeviceImpl& ioimpl();
};

} // namespace Net

} // namespace Pt

#endif
