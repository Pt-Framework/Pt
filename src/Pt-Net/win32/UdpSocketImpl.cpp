/*
 * Copyright (C) 2010 Marc Boris Duerner
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

#include "UdpSocketImpl.h"
#include "MainLoopImpl.h"
#include "Pt/Net/AddrInfo.h"
#include "Pt/Net/AddressInUse.h"
#include <Pt/Net/UdpSocket.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/IOError.h>
#include <limits>
#include <cstring>
#include <cassert>

namespace Pt {

namespace Net {

UdpSocketImpl::UdpSocketImpl(UdpSocket& socket)
: _ioh(socket)
, _fd(INVALID_SOCKET)
, _broadcast(false)
, _isConnected(false)
, _isBound(false)
, _eventFlags(FD_CLOSE)
, _timeout(Pt::System::EventLoop::WaitInfinite)
{
}


UdpSocketImpl::~UdpSocketImpl()
{
}


void UdpSocketImpl::setEventFlags(HANDLE ev, long events)
{
    if( WSAEventSelect(_fd, ev, events) == SOCKET_ERROR )
    {
        throw System::SystemError("WSAEventSelectt failed");
    }
}


void UdpSocketImpl::cancel(System::EventLoop& loop)
{
    if(_ioh.handle() != INVALID_HANDLE_VALUE)
    {
        loop.selector().disableOverlapped(_ioh);
    }

    _eventFlags = 0;
    if( _fd != INVALID_SOCKET )
    {
        this->setEventFlags(0, 0);
    }
}


void UdpSocketImpl::close()
{
    if( _fd == INVALID_SOCKET )
        return;

    ::closesocket(_fd);
    _fd = INVALID_SOCKET;
    _isConnected = false;
    _isBound = false;
}


void UdpSocketImpl::bind(const std::string& ipaddr, unsigned short int port, unsigned flags)
{
    AddrInfo ai(ipaddr, port, true);

    BOOL reuseAddr = TRUE;
    bool addrInUse = false;

    for (AddrInfoImpl::const_iterator it = ai.impl()->begin(); it != ai.impl()->end(); ++it)
    {
        if( _isConnected )
        {
            if(it->ai_family != _peeraddr.ss_family)
                continue;
        }
        else if( _isBound )
        {
            if(it->ai_family != _servaddr.ss_family)
                this->close();
        }

        if( it->ai_family == AF_INET6 && _broadcast)
            continue;

        if( _fd == INVALID_SOCKET )
            _fd = WSASocket(it->ai_family, SOCK_DGRAM, 0, NULL, 0, 0);

        if( _fd == INVALID_SOCKET )
            continue;

        if (::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddr, sizeof(reuseAddr)) < 0)
        {
            this->close();
            throw System::SystemError("setsockopt");
        }

#if defined(IPV6_V6ONLY)
        const int on = 1;

        if( it->ai_family == AF_INET6 )
        {
            if( ::setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, (const char*) &on, sizeof(on)) < 0 )
            {
                this->close();
                throw System::SystemError("setsockopt IPV6_V6ONLY failed");
            }
        }
#endif

        if( ::bind(_fd, it->ai_addr, it->ai_addrlen) == 0 )
        {
            _isBound = true;
            std::memmove(&_servaddr, it->ai_addr, it->ai_addrlen);
            return;
        }

        addrInUse = WSAGetLastError() == WSAEADDRINUSE;

        if( ! _isConnected )
            this->close();
    }

    if(addrInUse)
        throw AddressInUse();
    else
        throw System::AccessFailed( ai.host() );
}


void UdpSocketImpl::connect(const AddrInfo& ai)
{
    AddrInfoImpl::const_iterator it = ai.impl()->begin();

    for( ; it != ai.impl()->end(); ++it)
    {
        if( _isBound )
        {
            if(it->ai_family != _servaddr.ss_family)
                continue;
        }
        else if( _isConnected )
        {
            if(it->ai_family != _peeraddr.ss_family)
                this->close();
        }

        if( _fd == INVALID_SOCKET )
            _fd = WSASocket(it->ai_family, SOCK_DGRAM, 0, NULL, 0, 0);

        if( _fd == INVALID_SOCKET )
            continue;

        if(_broadcast)
        {
            const int on = 1;
            if (::setsockopt(_fd, SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on)) < 0)
            {
                this->close();
                throw System::SystemError("setsockopt");
            }
        }

        std::memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);

        if( 0 == ::connect(_fd, it->ai_addr, it->ai_addrlen) )
        {
            _isConnected = true;
            return;
        }

        if( ! _isBound )
            this->close();
    }

    throw System::AccessFailed( ai.host() );
}


bool UdpSocketImpl::isConnected() const
{
    return _isConnected;
}


bool UdpSocketImpl::isBound() const
{
    return _isBound;
}


void UdpSocketImpl::setBroadcast()
{
    _broadcast = true;
}


void UdpSocketImpl::joinMulticastGroup(const std::string& ipaddr)
{
    if( _fd == INVALID_SOCKET )
        return;

    AddrInfo ai(ipaddr, 0, true);
    for(AddrInfoImpl::const_iterator it = ai.impl()->begin(); it != ai.impl()->end(); ++it)
    {
        if(it->ai_family == AF_INET)
        {
            ip_mreq req;

            sockaddr_in* sa = (sockaddr_in*)(it->ai_addr);
            memcpy( &req.imr_multiaddr, &sa->sin_addr, sizeof(struct in_addr) );

            req.imr_interface.s_addr = htonl(INADDR_ANY);

            if (::setsockopt(_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&req, sizeof(ip_mreq)) == 0)
            {
                return; // success
            }
        }
        else if(it->ai_family == AF_INET6)
        {
            ipv6_mreq req;
            sockaddr_in6* sa = (sockaddr_in6*)(it->ai_addr);
            memcpy( &req.ipv6mr_multiaddr, &sa->sin6_addr, sizeof(struct in6_addr) );

            req.ipv6mr_interface = 0;

            if (::setsockopt(_fd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char*)&req, sizeof(ipv6_mreq)) == 0)
            {
                return; // success
            }
        }
    }

    throw System::IOError("multicast group join failed");
}


void UdpSocketImpl::dropMulticastGroup(const std::string& ipaddr)
{
    if( _fd == INVALID_SOCKET )
        return;

    AddrInfo ai(ipaddr, 0, true);
    for(AddrInfoImpl::const_iterator it = ai.impl()->begin(); it != ai.impl()->end(); ++it)
    {
        if(it->ai_family == AF_INET)
        {
            ip_mreq req;

            sockaddr_in* sa = (sockaddr_in*)(it->ai_addr);
            memcpy( &req.imr_multiaddr, &sa->sin_addr, sizeof(struct in_addr) );

            req.imr_interface.s_addr = htonl(INADDR_ANY);

            if (::setsockopt(_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&req, sizeof(ip_mreq)) == 0)
            {
                return; // success
            }
        }
        else if(it->ai_family == AF_INET6)
        {
            ipv6_mreq req;
            sockaddr_in6* sa = (sockaddr_in6*)(it->ai_addr);
            memcpy( &req.ipv6mr_multiaddr, &sa->sin6_addr, sizeof(struct in6_addr) );

            req.ipv6mr_interface = 0;

            if (::setsockopt(_fd, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, (char*)&req, sizeof(ipv6_mreq)) == 0)
            {
                return; // success
            }
        }
    }

    throw System::IOError("multicast group drop failed");
}


std::string UdpSocketImpl::socketAddress() const
{
    std::string address;

    if(this->isBound() )
    {
        SOCKADDR* saddr = reinterpret_cast<SOCKADDR*>(&_servaddr);
    
        DWORD len = 32;
        TCHAR adr[32];
        WSAAddressToString(saddr, sizeof(SOCKADDR), NULL, adr, &len);
    
        for(unsigned n = 0; n < len; n++)
            address.push_back( int(adr[n]) );
    }

    return address;
}


std::string UdpSocketImpl::peerAddress() const
{
    std::string address;

    if(this->isConnected() )
    {
        SOCKADDR* saddr = reinterpret_cast<SOCKADDR*>(&_peeraddr);
    
        DWORD len = 32;
        TCHAR adr[32];
        WSAAddressToString(saddr, sizeof(SOCKADDR), NULL, adr, &len);
    
        for(unsigned n = 0; n < len; n++)
            address.push_back( int(adr[n]) );
    }

    return address;
}


bool UdpSocketImpl::runRead(System::EventLoop& loop)
{
    WSANETWORKEVENTS events;

    if( WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR )
        throw System::SystemError("WSAEnumNetworkEvents failed");

    if( (events.lNetworkEvents & FD_READ) == FD_READ )
    {
        return true;
    }

    return false;
}


bool UdpSocketImpl::runWrite(System::EventLoop& loop)
{
    WSANETWORKEVENTS events;

    if( WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR )
        throw System::SystemError("WSAEnumNetworkEvents failed");

    if( (events.lNetworkEvents & FD_WRITE) == FD_WRITE )
    {
       return true;
    }

    return false;
}


size_t UdpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
    WSABUF recvbuf;
    recvbuf.buf = buffer;
    recvbuf.len = count;

    int addrlen = sizeof(_peeraddr);
    int len = recvfrom( _fd, recvbuf.buf, recvbuf.len, 
                        0, (sockaddr*)&_peeraddr, &addrlen );

    if( len == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
    {
        //Set socket to blocking mode
        setEventFlags(0,0);

        u_long argp = 0;
        ::ioctlsocket(_fd, FIONBIO, &argp);

        len = recvfrom( _fd, recvbuf.buf, recvbuf.len, 0,
                        (sockaddr*) &_peeraddr,  &addrlen );

        //Set socket to non-blocking mode
        argp = 1;
        ::ioctlsocket(_fd, FIONBIO, &argp);

        if(_ioh.handle() != INVALID_HANDLE_VALUE)
            setEventFlags(_ioh.handle(), _eventFlags);

        if(len < 0)
            throw System::IOError("recvfrom");
    }

    return len;
}


size_t UdpSocketImpl::beginRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    if(_ioh.handle() == INVALID_HANDLE_VALUE)
    {
        loop.selector().enableOverlapped(_ioh);
    }

    assert(buffer != 0);
    _eventFlags |= FD_READ;

    _receiveBuffer.buf = buffer;
    _receiveBuffer.len = n;

    setEventFlags(_ioh.handle(), _eventFlags);
    return 0;
}


size_t UdpSocketImpl::endRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    _eventFlags &= ~FD_READ;
    setEventFlags(_ioh.handle(), _eventFlags);

    int addrlen = sizeof(_peeraddr);
    int len = recvfrom( _fd, _receiveBuffer.buf, _receiveBuffer.len, 0,
                        (sockaddr*) &_peeraddr, &addrlen );

    if( len == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
    {
        //Set socket to blocking mode
        setEventFlags(0,0);

        u_long argp = 0;
        ::ioctlsocket(_fd, FIONBIO, &argp);

        len = recvfrom( _fd, _receiveBuffer.buf, _receiveBuffer.len, 0,
                        (sockaddr*) &_peeraddr,  &addrlen );

        //Set socket to non-blocking mode
        argp = 1;
        ::ioctlsocket(_fd, FIONBIO, &argp);

        setEventFlags(_ioh.handle(), _eventFlags);

        if(len < 0)
            throw System::IOError("recvfrom");
    }

    return len;
}


size_t UdpSocketImpl::write(const char* buffer, size_t n)
{
    WSABUF sendbuf;
    sendbuf.buf = const_cast<char*>(buffer);
    sendbuf.len = n;

    //Set socket to blocking mode
    setEventFlags(0, 0);
    u_long argp = 0;
    ::ioctlsocket(_fd, FIONBIO, &argp);

    DWORD bytesSent = 0;
    int rc = WSASend(_fd, &sendbuf, 1, &bytesSent, 0, NULL, NULL);

    //Set socket to non-blocking mode
    argp = 1;
    ::ioctlsocket(_fd, FIONBIO, &argp);

    if(_ioh.handle() != INVALID_HANDLE_VALUE)
        setEventFlags(_ioh.handle(), _eventFlags);

    if(rc == SOCKET_ERROR)
        throw System::IOError("WSASend");  

    return  bytesSent;
}


size_t UdpSocketImpl::beginWrite(System::EventLoop& loop, const char* buffer, size_t n)
{
    if(_ioh.handle() == INVALID_HANDLE_VALUE)
    {
        loop.selector().enableOverlapped(_ioh);
    }

     _sendBuffer.buf = const_cast<char*>(buffer);
    _sendBuffer.len = n;

    DWORD numberOfBytesSent = 0;

    int rc = WSASendTo( _fd, &_sendBuffer, 1, &numberOfBytesSent, 0,
                        (sockaddr*)&_peeraddr, sizeof(_peeraddr), NULL, NULL);

    if(rc == SOCKET_ERROR)
    {
        if(WSAGetLastError() == WSAEWOULDBLOCK)
        {
            _eventFlags |= FD_WRITE;
            setEventFlags(_ioh.handle(), _eventFlags);
            return 0;
        }
    }

    return numberOfBytesSent;
}


size_t UdpSocketImpl::endWrite(System::EventLoop& loop, const char* buffer, size_t n)
{
    _eventFlags &= ~FD_WRITE;

    // The WSAEventSelect function automatically sets socket s to nonblocking
    // mode, regardless of the value of lNetworkEvents. To set socket s back 
    // to blocking mode, it is first necessary to clear the event record 
    // associated with socket s via a call to WSAEventSelect with 
    // lNetworkEvents set to zero and the hEventObject parameter set to NULL.
    // You can then call ioctlsocket or WSAIoctl to set the socket back to blocking mode.
    setEventFlags(0, 0);

    u_long argp = 0;
    ::ioctlsocket(_fd, FIONBIO, &argp);

    DWORD bytesSend = 0;
    int rc = WSASend(_fd, &_sendBuffer, 1, &bytesSend, 0, NULL, NULL);

    //Set socket to non-blocking mode
    argp = 1;
    ::ioctlsocket(_fd, FIONBIO, &argp);
    setEventFlags(_ioh.handle(), _eventFlags);

    if(rc == SOCKET_ERROR)
        throw System::IOError("WSASend");

    return  bytesSend;
}

} // namespace Net

} // namespace Pt

