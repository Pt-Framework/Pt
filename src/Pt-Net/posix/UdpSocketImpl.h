/*
 * Copyright (C) 2010 by Marc Boris Duerner
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

#ifndef PT_NET_UdpSocketImpl_H
#define PT_NET_UdpSocketImpl_H

#include "IODeviceImpl.h"
#include <Pt/Net/Api.h>
#include <Pt/Net/UdpSocket.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>

// InterfaceInfo
#include <cstring>
#include <sys/ioctl.h> 
#include <net/if.h>

#include <ifaddrs.h>
struct ifreq;
struct ifaddrs;

namespace Pt {

namespace Net {

class UdpSocket;

struct InterfaceInfo
{
    InterfaceInfo()
    : _adapters(0)
    {
        getifaddrs(&_adapters);
    }
    
    ~InterfaceInfo()
    { freeifaddrs(_adapters); }
    
    ifaddrs* adapters()
    { return _adapters; }
    
    struct ifaddrs* _adapters;
};

// name to iface address:
//
// use if_nameindex to get a list of device names

class InterfaceInfo2
{
    public:
        class Iterator
        {
            public:
                explicit Iterator(int sock = 0, ifconf* ifc = 0, ifreq* ifr = 0)
                : _sock(sock)
                , _ifc(ifc)
                , _offset(0)
                , _current(ifr)
                { }

                bool operator== (const Iterator& it) const
                { return _current == it._current; }

                bool operator!= (const Iterator& it) const
                { return _current != it._current; }

                Iterator& operator++()
                { 
                #ifdef __linux__
                    size_t len = sizeof(ifreq);
                #else
                    size_t len = _current->ifr_addr.sa_len;
                    len += sizeof(_current->ifr_name);
                    len = std::max(sizeof(ifreq), len);
                #endif

                    _offset += len;
                    
                    if(_offset < _ifc->ifc_len)
                        _current = (struct ifreq*)((char*)_current + len);
                    else
                        _current = 0;
                    
                    return *this; 
                }

                const char* operator*() const
                { 
                    return _current->ifr_name; 
                }

                int index()
                {
                    return if_nametoindex(_current->ifr_name);
                }

                sockaddr* addr()
                {
                    std::strncpy(_ifr.ifr_name, _current->ifr_name, IFNAMSIZ);
                    
                    ioctl( _sock, SIOCGIFADDR, &_ifr);
                    return &_ifr.ifr_addr;
                }

            private:
                int _sock;
                ifconf* _ifc;
                int     _offset;
                ifreq*  _current;
                ifreq   _ifr;
        };

    public:
        InterfaceInfo2(int sock);

        ~InterfaceInfo2();

        Iterator begin()
        { return Iterator(_sock, &_ifc, _ifr); }

        Iterator end()
        { return Iterator(); }

    private:
        int _sock;
        ifreq  _ifr[20];
        ifconf _ifc;
};


class InterfaceInfo3
{
    public:
        class Iterator
        {
            public:
                explicit Iterator(ifaddrs* ifa = 0)
                : _current(ifa)
                { }
            
                bool operator== (const Iterator& it) const
                { return _current == it._current; }

                bool operator!= (const Iterator& it) const
                { return _current != it._current; }

                Iterator& operator++ ()
                { 
                    _current = _current->ifa_next; 
                    return *this; 
                }

                const char* operator*() const
                { 
                    return _current->ifa_name; 
                }

                int index()
                {
                    return if_nametoindex(_current->ifa_name);
                }

                sockaddr* addr()
                {
                    return _current->ifa_addr;
                }

            private:
                ifaddrs* _current;
        };
    
    public:
        InterfaceInfo3(int sock);

        ~InterfaceInfo3();

        Iterator begin()
        { return Iterator(_adapters); }

        Iterator end()
        { return Iterator(); }

    private:
        ifaddrs* _adapters;
};


class UdpSocketImpl : public System::IODeviceImpl
{
    public:
        UdpSocketImpl(UdpSocket& socket);

        ~UdpSocketImpl();

        void close();

        bool beginBind(System::EventLoop& loop, const Endpoint& ep, const UdpSocketOptions& o);

        bool runBind(System::EventLoop& loop);

        void endBind(System::EventLoop& loop);

        void bind(const Endpoint& ep, const UdpSocketOptions& o);

        bool beginConnect(System::EventLoop& loop, const Endpoint& ep, const UdpSocketOptions& o);

        bool runConnect(System::EventLoop& loop);

        void endConnect(System::EventLoop& loop);

        void connect(const Endpoint& ep, const UdpSocketOptions& o);

        void setTarget(const Endpoint& ep, const UdpSocketOptions& o);

        bool isConnected() const;

        bool isBound() const;

        void setBroadcast();

        void joinMulticastGroup(const std::string& ipaddr);

        void dropMulticastGroup(const std::string& ipaddr);

        void localEndpoint(Endpoint& ep) const;

        const Endpoint& remoteEndpoint() const;

        size_t beginRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof);

        size_t read(char* buffer, size_t count, bool& eof);

        size_t beginWrite(System::EventLoop& loop, const char* buffer, size_t n);

        size_t write(const char* buffer, size_t n);

    private:
        bool             _isConnected;
        bool             _isBound;
        Endpoint         _peerAddr;
        sockaddr_storage _servaddr;
        sockaddr_storage _sendaddr;
        socklen_t        _sendaddrLen;
};

} // namespace Net

} // namespace Pt

#endif
