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
#include <Pt/Net/AddrInfo.h>
#include <Pt/Net/UdpSocket.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/IOError.h>
#include <cerrno>
#include <stdio.h>
#include <errno.h>
#include <cstring>
#include <cassert>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Pt {

namespace Net {

UdpSocketImpl::UdpSocketImpl(UdpSocket& socket)
: System::IODeviceImpl(socket)
, _broadcast(false)
, _isConnected(false)
, _isBound(false)
{
}


UdpSocketImpl::~UdpSocketImpl()
{
//  assert(_rfds == 0);
//  assert(_wfds == 0);

    if(_sentry)
        _sentry->detach();
}


void UdpSocketImpl::close(System::EventLoop* loop)
{
    System::IODeviceImpl::close(loop);
    _isConnected = false;
    _isBound = false;
}


void UdpSocketImpl::bind(const std::string& ipaddr, unsigned short int port, unsigned flags, System::EventLoop* loop)
{
    AddrInfo ai(ipaddr, port, true);

    const int on = 1;
    bool addrInUse = false;

    for (AddrInfoImpl::const_iterator it = ai.impl()->begin(); it != ai.impl()->end(); ++it)
    {
        if( _isConnected )
        {
            if(it->ai_family != reinterpret_cast <struct sockaddr*>(&_peeraddr)->sa_family)
                continue;
        }
        else if( _isBound )
        {
            if(it->ai_family != reinterpret_cast <struct sockaddr*>(&_servaddr)->sa_family)
                this->close(loop);
        }

        if(it->ai_family == AF_INET6 && _broadcast )
            continue;

        if( this->fd() < 0 )
        {
            int fd = socket(it->ai_family, SOCK_DGRAM, 0);
            IODeviceImpl::open(fd, false, loop);
        }

        if( this->fd() < 0 )
            continue;

#ifdef SO_REUSEPORT
        if (::setsockopt(this->fd(), SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(on)) < 0)
        {
            this->close(loop);
            throw System::SystemError("setsockopt SO_REUSEPORT");
        }
#endif

        if (::setsockopt(this->fd(), SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0)
        {
            this->close(loop);
            throw System::SystemError("setsockopt SO_REUSEADDR");
        }

#if defined(IPV6_V6ONLY)
        if( it->ai_family == AF_INET6 )
        {
            if( ::setsockopt(this->fd(), IPPROTO_IPV6, IPV6_V6ONLY, (const char*) &on, sizeof(on)) < 0 )
            {
                this->close(loop);
                throw System::SystemError("setsockopt IPV6_V6ONLY failed");
            }
        }
#endif

        if( ::bind(this->fd(), it->ai_addr, it->ai_addrlen) == 0 )
        {
            _isBound = true;
            std::memmove(&_servaddr, it->ai_addr, it->ai_addrlen);
            return;
        }

        addrInUse = errno == EADDRINUSE;

        if( ! _isConnected )
            this->close(loop);
    }

    if(addrInUse)
        throw AddressInUse();
    else
        throw System::IOError("bind");
}


void UdpSocketImpl::connect(const AddrInfo& ai, System::EventLoop* loop)
{
    AddrInfoImpl::const_iterator it = ai.impl()->begin();
    for( ; it != ai.impl()->end(); ++it)
    {
        if( _isBound )
        {
            if(it->ai_family != reinterpret_cast <struct sockaddr*>(&_servaddr)->sa_family)
                continue;
        }
        else if( _isConnected )
        {
            if(it->ai_family != reinterpret_cast <struct sockaddr*>(&_peeraddr)->sa_family)
                this->close(loop);
        }

        if( this->fd() < 0 )
        {
            int fd = socket(it->ai_family, SOCK_DGRAM, 0);
            IODeviceImpl::open(fd, false, loop);
        }

        if( this->fd() < 0 )
            continue;

        if( _broadcast)
        {
            const int on = 1;
            if( 0 > ::setsockopt(this->fd(), SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on)) )
            {
                if( ! _isBound )
                    this->close(loop);

                throw System::SystemError("setsockopt SO_BROADCAST failed");
            }
        }

        std::memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);

        if( 0 == ::connect(this->fd(), it->ai_addr, it->ai_addrlen) )
        {
            _isConnected = true;
            return;
        }

        if( ! _isBound )
            this->close(loop);
    }

    throw System::IOError("connect failed");
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
    if( this->fd() < 0 )
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

            if (::setsockopt(this->fd(), IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&req, sizeof(ip_mreq)) == 0)
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

            if (::setsockopt(this->fd(), IPPROTO_IPV6, IPV6_JOIN_GROUP, (char*)&req, sizeof(ipv6_mreq)) == 0)
            {
                return; // success
            }
        }
    }


    throw System::IOError("multicast group join failed");
}


void UdpSocketImpl::dropMulticastGroup(const std::string& ipaddr)
{
    if( this->fd() < 0 )
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

            if (::setsockopt(this->fd(), IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&req, sizeof(ip_mreq)) == 0)
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

            if (::setsockopt(this->fd(), IPPROTO_IPV6, IPV6_LEAVE_GROUP, (char*)&req, sizeof(ipv6_mreq)) == 0)
            {
                return; // success
            }
        }
    }

    throw System::IOError("multicast group drop failed");
}


std::string UdpSocketImpl::getSockAddr() const
{
	const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&_servaddr);
    char strbuf[INET6_ADDRSTRLEN + 1];
    const char* p = inet_ntop(sa->sin_family, &sa->sin_addr, strbuf, sizeof(strbuf));
    return p ? strbuf: "-";
}


std::string UdpSocketImpl::getPeerAddr() const
{
	const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&_peeraddr);
    char strbuf[INET6_ADDRSTRLEN + 1];
    const char* p = inet_ntop(sa->sin_family, &sa->sin_addr, strbuf, sizeof(strbuf));
    return p ? strbuf: "-";
}


size_t UdpSocketImpl::read( char* buffer, size_t count, bool& eof )
{
    ssize_t ret = 0;
    socklen_t addrlen = sizeof(_peeraddr);

    while(true)
    {
        ret = ::recvfrom( this->fd(), buffer, count, 0, (sockaddr*)&_peeraddr, &addrlen );
        if(ret > 0)
            break;

        if(errno == EINTR)
            continue;

        if(errno != EAGAIN)
            throw System::IOError("read failed", PT_SOURCEINFO);

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(this->fd(), &rfds);
        bool ret = this->wait(_timeout, &rfds, 0, 0);
        if(false == ret)
        {
            throw System::IOTimeout();
        }
    }

    return ret;
}


size_t UdpSocketImpl::beginWrite(System::EventLoop& loop, const char* buffer, size_t n)
{
    ssize_t ret = ::sendto( this->fd(), buffer, n, 0, (sockaddr*)&_peeraddr, sizeof(_peeraddr));

    if (ret > 0)
        return static_cast<size_t>(ret);

    if (ret == 0 || errno == ECONNRESET || errno == EPIPE)
        throw System::IOError("lost connection to peer");

    std::cerr << "IODeviceImpl::beginWrite on handle " << std::endl;
    loop.impl().beginWrite( &_ioh );

    return 0;
}


size_t UdpSocketImpl::write( const char* buffer, size_t count )
{
    ssize_t ret = 0;

    while(true)
    {
        if(_isConnected)
            ret = ::write( this->fd(), buffer, count);
        else
            ret = ::sendto( this->fd(), buffer, count, 0, (sockaddr*)&_peeraddr, sizeof(_peeraddr));

        if(ret >= 0)
            break;

        if(errno == EINTR)
            continue;

        if(errno != EAGAIN)
        {
            throw System::IOError("udp socket I/O failed");
        }

        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(this->fd(), &wfds);

        if(false == this->wait(_timeout, 0, &wfds, 0) )
        {
            throw System::IOTimeout();
        }
    }

    return ret;
}

} // namespace Net

} // namespace Pt
