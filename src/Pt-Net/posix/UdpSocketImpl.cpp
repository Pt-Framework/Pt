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
#include "AddrInfo.h"
#include "EndpointImpl.h"
#include <Pt/Net/AddressInUse.h>
#include <Pt/Net/UdpSocket.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>
#include <cerrno>
#include <stdio.h>
#include <errno.h>
#include <cstring>
#include <cassert>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

log_define("Pt.Net.UdpSocket");

namespace Pt {

namespace Net {

UdpSocketImpl::UdpSocketImpl(UdpSocket& socket)
: System::IODeviceImpl(socket)
, _isConnected(false)
, _isBound(false)
, _sendaddrLen(0)
{
}


UdpSocketImpl::~UdpSocketImpl()
{
//  assert(_rfds == 0);
//  assert(_wfds == 0);

    if(_sentry)
        _sentry->detach();
}


void UdpSocketImpl::close()
{
    System::IODeviceImpl::close();
    _isConnected = false;
    _isBound = false;
    
    _sendaddrLen = 0;
}


bool UdpSocketImpl::beginBind(System::EventLoop& loop, const Endpoint& ep, const UdpSocketOptions& o)
{
    log_debug( "begin binding socket to " << ep.toString() );

    this->bind(ep, o);
    return true;
}


bool UdpSocketImpl::runBind(System::EventLoop& loop)
{
    return false;
}


void UdpSocketImpl::endBind(System::EventLoop& loop)
{
}


void UdpSocketImpl::bind(const Endpoint& ep, const UdpSocketOptions& opts)
{
    const int on = 1;
    bool addrInUse = false;
    
    AddrInfo ai;
    ai.resolve(ep, true);

    for (AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        if( _isConnected )
        {
            if(it->ai_family != _sendaddr.ss_family)
                continue;
        }
        else if( _isBound )
        {
            if(it->ai_family != _servaddr.ss_family)
                this->close();
        }

        if(it->ai_family == AF_INET6 && opts.isBroadcast() )
            continue;

        if( this->fd() < 0 )
        {
            int fd = socket(it->ai_family, SOCK_DGRAM, 0);
            IODeviceImpl::open(fd, false);
        }

        if( this->fd() < 0 )
            continue;

#ifdef SO_REUSEPORT
        if (::setsockopt(this->fd(), SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(on)) < 0)
        {
            this->close();
            throw System::SystemError("setsockopt SO_REUSEPORT");
        }
#endif

        if (::setsockopt(this->fd(), SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0)
        {
            this->close();
            throw System::SystemError("setsockopt SO_REUSEADDR");
        }

#if defined(IPV6_V6ONLY)
        if( it->ai_family == AF_INET6 )
        {
            if( ::setsockopt(this->fd(), IPPROTO_IPV6, IPV6_V6ONLY, (const char*) &on, sizeof(on)) < 0 )
            {
                this->close();
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
            this->close();
    }

    if(addrInUse)
        throw AddressInUse( ep.toString() );
    else
        throw System::AccessFailed( ep.toString() );
}


bool UdpSocketImpl::beginConnect(System::EventLoop& loop, const Endpoint& ep, const UdpSocketOptions& o)
{
    log_debug( "begin connecting socket to " << ep.toString() );

    this->connect(ep, o);
    return true;
}


bool UdpSocketImpl::runConnect(System::EventLoop& loop)
{
    return false;
}


void UdpSocketImpl::endConnect(System::EventLoop& loop)
{
}


void UdpSocketImpl::connect(const Endpoint& ep, const UdpSocketOptions& opts)
{
    AddrInfo ainfo;
    ainfo.resolve(ep);

    AddrInfo::const_iterator it = ainfo.begin();

    for( ; it != ainfo.end(); ++it)
    {
        if( _isBound )
        {
            if(it->ai_family != _servaddr.ss_family);
                continue;
        }
        else if( _isConnected )
        {
            if(it->ai_family != _sendaddr.ss_family);
                this->close();
        }

        if( this->fd() < 0 )
        {
            int fd = socket(it->ai_family, SOCK_DGRAM, 0);
            IODeviceImpl::open(fd, false);
        }

        if( this->fd() < 0 )
            continue;

        if( opts.isBroadcast() )
        {
            const int on = 1;
            if( 0 > ::setsockopt(this->fd(), SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on)) )
            {
                if( ! _isBound )
                    this->close();

                throw System::SystemError("setsockopt SO_BROADCAST failed");
            }
        }

        std::memmove(&_sendaddr, it->ai_addr, it->ai_addrlen);
        _sendaddrLen = it->ai_addrlen;

        if( 0 == ::connect(this->fd(), it->ai_addr, it->ai_addrlen) )
        {
            _isConnected = true;
            return;
        }

        if( ! _isBound )
            this->close();
    }

    throw System::AccessFailed( ep.toString() );
}


void UdpSocketImpl::setTarget(const Endpoint& ep, const UdpSocketOptions& opts)
{
    AddrInfo ainfo;
    ainfo.resolve(ep);

    AddrInfo::const_iterator it = ainfo.begin();

    for( ; it != ainfo.end(); ++it)
    {
        if( _isBound )
        {
            if(it->ai_family != _servaddr.ss_family)
                continue;
        }
        else if( _isConnected )
        {
            if(it->ai_family != _sendaddr.ss_family)
                this->close();
        }

        if( this->fd() < 0 )
        {
            int fd = socket(it->ai_family, SOCK_DGRAM, 0);
            IODeviceImpl::open(fd, false);
        }

        if( this->fd() < 0 )
            continue;

        if( opts.isBroadcast() )
        {
            const int on = 1;
            if( 0 > ::setsockopt(this->fd(), SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on)) )
            {
                if( ! _isBound )
                    this->close();

                throw System::SystemError("setsockopt SO_BROADCAST failed");
            }
        }

        std::memmove(&_sendaddr, it->ai_addr, it->ai_addrlen);
        _sendaddrLen = it->ai_addrlen;
        return;
    }

    throw System::AccessFailed( ep.toString() );
}


bool UdpSocketImpl::isConnected() const
{
    return _isConnected;
}


bool UdpSocketImpl::isBound() const
{
    return _isBound;
}


void UdpSocketImpl::joinMulticastGroup(const std::string& ipaddr)
{
    if( this->fd() < 0 )
        return;

    Endpoint ep(ipaddr, 0);

    AddrInfo ai;
    ai.resolve(ep, true);

    for(AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        if(it->ai_family == AF_INET)
        {
            ip_mreq req;

            sockaddr_in* sa = (sockaddr_in*)(it->ai_addr);
            std::memcpy( &req.imr_multiaddr, &sa->sin_addr, sizeof(struct in_addr) );

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
            std::memcpy( &req.ipv6mr_multiaddr, &sa->sin6_addr, sizeof(struct in6_addr) );

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

    Endpoint ep(ipaddr, 0);

    AddrInfo ai;
    ai.resolve(ep, true);

    for(AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        if(it->ai_family == AF_INET)
        {
            ip_mreq req;

            sockaddr_in* sa = (sockaddr_in*)(it->ai_addr);
            std::memcpy( &req.imr_multiaddr, &sa->sin_addr, sizeof(struct in_addr) );

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
            std::memcpy( &req.ipv6mr_multiaddr, &sa->sin6_addr, sizeof(struct in6_addr) );

            req.ipv6mr_interface = 0;

            if (::setsockopt(this->fd(), IPPROTO_IPV6, IPV6_LEAVE_GROUP, (char*)&req, sizeof(ipv6_mreq)) == 0)
            {
                return; // success
            }
        }
    }

    throw System::IOError("multicast group drop failed");
}


void UdpSocketImpl::localEndpoint(Endpoint& ep) const
{      
    struct sockaddr_storage addr;
    socklen_t slen = sizeof(addr);

    int ret = ::getsockname(fd(), reinterpret_cast<struct sockaddr*>(&addr), &slen);

    if(ret == 0)
        ep.impl()->init( (sockaddr*)&addr, slen );
    else
        ep.clear();

    //ep.impl()->init( (sockaddr*)&_servaddr, sizeof(_servaddr) );
}


const Endpoint& UdpSocketImpl::remoteEndpoint() const
{
    return _peerAddr;
}


std::size_t UdpSocketImpl::beginRead(System::EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    log_debug("begin read on fd:" << _ioh.fd);

    sockaddr_storage peerAddr;
    socklen_t addrlen = sizeof(peerAddr);
    sockaddr* addr = reinterpret_cast<sockaddr*>(&peerAddr);

    for(;;)
    {
        ssize_t ret = ::recvfrom( this->fd(), buffer, n, 0, addr, &addrlen );
        if (ret > 0)
        {
            log_debug("read:" << ret << " bytes");
            
            _peerAddr.impl()->init(addr, addrlen);
            return static_cast<std::size_t>(ret);
        }

        if(errno == EAGAIN || errno == EWOULDBLOCK)
            break;

        if(errno != EINTR)
            throw System::IOError("read failed");
    }

    loop.selector().beginRead( &_ioh );
    return 0;
}


std::size_t UdpSocketImpl::read( char* buffer, std::size_t count, bool& eof )
{
    ssize_t ret = 0;

    sockaddr_storage peerAddr;
    socklen_t addrlen = sizeof(peerAddr);
    sockaddr* addr = reinterpret_cast<sockaddr*>(&peerAddr);

    while(true)
    {
        ret = ::recvfrom( this->fd(), buffer, count, 0, addr, &addrlen );
        if(ret > 0)
            break;

        if(errno == EINTR)
            continue;

        if(errno != EAGAIN)
            throw System::IOError("read failed");

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(this->fd(), &rfds);
        bool ret = this->wait(_timeout, &rfds, 0, 0);
        if(false == ret)
        {
            throw System::IOError("recvfrom");
        }
    }

    _peerAddr.impl()->init(addr, addrlen);
    return ret;
}


std::size_t UdpSocketImpl::beginWrite(System::EventLoop& loop, const char* buffer, std::size_t n)
{
    ssize_t ret = 0;

    if(_isConnected)
        ret = ::write( this->fd(), buffer, n);
    else
        ret = ::sendto( this->fd(), buffer, n, 0, (sockaddr*)&_sendaddr, _sendaddrLen);

    if (ret > 0)
        return static_cast<std::size_t>(ret);

    if (ret == 0 || errno == ECONNRESET || errno == EPIPE)
        throw System::IOError("lost connection to peer");

    loop.selector().beginWrite( &_ioh );

    return 0;
}


std::size_t UdpSocketImpl::write( const char* buffer, std::size_t count )
{
    ssize_t ret = 0;

    while(true)
    {
        if(_isConnected)
            ret = ::write( this->fd(), buffer, count);
        else
            ret = ::sendto( this->fd(), buffer, count, 0, (sockaddr*)&_sendaddr, _sendaddrLen);

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
            throw System::IOError("recvfrom");
        }
    }

    return ret;
}

} // namespace Net

} // namespace Pt
