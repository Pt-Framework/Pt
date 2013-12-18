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
#include "EndpointImpl.h"
#include "AddrInfo.h"
#include "Pt/Net/AddressInUse.h"
#include <Pt/Net/UdpSocket.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>
#include <limits>
#include <cstring>
#include <cassert>

log_define("Pt.Net.UdpSocket");

namespace Pt {

namespace Net {

const unsigned int DefaultHopLimit = static_cast<unsigned int>(-1);

UdpSocketImpl::UdpSocketImpl(UdpSocket& socket)
: _ioh(socket)
, _fd(INVALID_SOCKET)
, _isConnected(false)
, _isBound(false)
, _hopLimit(DefaultHopLimit)
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
        throw System::SystemError("WSAEventSelect failed");
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

    _hopLimit = DefaultHopLimit;
}


bool UdpSocketImpl::beginBind(System::EventLoop& loop, const Endpoint& ai, const UdpSocketOptions& o)
{
    log_debug( "begin binding socket to " << ai.toString() );

    this->bind(ai, o);
    return true;
}


bool UdpSocketImpl::runBind(System::EventLoop& loop)
{
    return false;
}


void UdpSocketImpl::endBind(System::EventLoop& loop)
{
    log_debug( "end bind" );
}


void UdpSocketImpl::bind(const Endpoint& e, const UdpSocketOptions& opts)
{
    log_debug( "bind socket to " << e.toString() );

    AddrInfo ai;
    ai.resolve(e, true);

    BOOL reuseAddr = TRUE;
    bool addrInUse = false;

    for (AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        if( _isConnected )
        {
            if(it->ai_family != _sendAddr.ss_family)
                continue;
        }
        else if( _isBound )
        {
            if(it->ai_family != _servaddr.ss_family)
                this->close();
        }

        if( it->ai_family == AF_INET6 && opts.isBroadcast() )
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

        socklen_t addrlen = static_cast<socklen_t>(it->ai_addrlen);
        
        if( ::bind(_fd, it->ai_addr, addrlen) == 0 )
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
        throw AddressInUse( e.toString() );
    else
        throw System::AccessFailed( ai.host() );
}


bool UdpSocketImpl::beginConnect(System::EventLoop& loop, const Endpoint& ai, const UdpSocketOptions& o)
{
    log_debug( "begin connecting socket to " << ai.toString() );

    this->connect(ai, o);
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
            if(it->ai_family != _servaddr.ss_family)
                continue;
        }
        else if( _isConnected )
        {
            if(it->ai_family != _sendAddr.ss_family)
                this->close();
        }

        if( _fd == INVALID_SOCKET )
            _fd = WSASocket(it->ai_family, SOCK_DGRAM, 0, NULL, 0, 0);

        if( _fd == INVALID_SOCKET )
            continue;

        if( opts.isBroadcast() )
        {
            const int on = 1;
            if (::setsockopt(_fd, SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on)) < 0)
            {
                this->close();
                throw System::SystemError("setsockopt");
            }
        }

        if(_hopLimit != DefaultHopLimit)
        {
            try
            {
                this->setHopLimit(_hopLimit);
            }
            catch(...)
            {
                this->close();
            }
        }

        std::memmove(&_sendAddr, it->ai_addr, it->ai_addrlen);

        socklen_t addrlen = static_cast<socklen_t>(it->ai_addrlen);

        if( 0 == ::connect(_fd, it->ai_addr, addrlen) )
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
            if(it->ai_family != _sendAddr.ss_family)
                this->close();
        }

        if( _fd == INVALID_SOCKET )
            _fd = WSASocket(it->ai_family, SOCK_DGRAM, 0, NULL, 0, 0);

        if( _fd == INVALID_SOCKET )
            continue;

        if( opts.isBroadcast() )
        {
            const int on = 1;
            if (::setsockopt(_fd, SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on)) < 0)
            {
                this->close();
                throw System::SystemError("setsockopt");
            }
        }

        if(_hopLimit != DefaultHopLimit)
        {
            try
            {
                this->setHopLimit(_hopLimit);
            }
            catch(...)
            {
                this->close();
            }
        }

        std::memmove(&_sendAddr, it->ai_addr, it->ai_addrlen);
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


void UdpSocketImpl::setHopLimit(unsigned int n)
{
    _hopLimit = n;

    if(_fd < 0 || n == DefaultHopLimit)
        return;

    // TODO: move this to UdpOptions

    sockaddr_storage* sa = _isBound ? &_servaddr : &_sendAddr;
    if(sa->ss_family == AF_INET)
    {
        unsigned char ttl = static_cast<unsigned char>(_hopLimit);

        if( 0 > ::setsockopt(_fd, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl) ) )
        {
            throw System::SystemError("setsockopt IP_MULTICAST_TTL");
        }
    }
#if defined(IPV6_V6ONLY)
    else if(sa->ss_family == AF_INET6)
    {
        if( 0 > ::setsockopt(_fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char*)&_hopLimit, sizeof(_hopLimit) ) )
        {
            throw System::SystemError("setsockopt IP_MULTICAST_TTL");
        }

    }
#endif
}


void UdpSocketImpl::joinMulticastGroup(const std::string& ipaddr)
{
    log_debug( "joining multicast group " << ipaddr );

    if( _fd == INVALID_SOCKET )
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
            memcpy( &req.imr_multiaddr, &sa->sin_addr, sizeof(struct in_addr) );

            req.imr_interface.s_addr = htonl(INADDR_ANY);

            if (::setsockopt(_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&req, sizeof(ip_mreq)) == 0)
            {
                log_debug( "joined multicast group ip4 " << ipaddr );
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
                log_debug( "joined multicast group ip6 " << ipaddr );
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

    Endpoint ep(ipaddr, 0);

    AddrInfo ai;
    ai.resolve(ep, true);

    for(AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
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


void UdpSocketImpl::localEndpoint(Endpoint& ep) const
{
    sockaddr_storage sockadr;
    int l = sizeof(sockadr);
    int ret = getsockname(_fd, (sockaddr*)&sockadr, &l);

    if(ret == 0)
        ep.impl()->init( (sockaddr*)&sockadr, l );
    else
        ep.clear();

    //ep.impl()->init( (sockaddr*)&_servaddr, sizeof(_servaddr) );
}


const Endpoint& UdpSocketImpl::remoteEndpoint() const
{
    return _peerAddr;
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


std::size_t UdpSocketImpl::read(char* buffer, std::size_t n, bool& eof)
{
    unsigned int maxLen = std::numeric_limits<int>::max();
    int buflen = n > maxLen ? static_cast<int>(maxLen) : static_cast<int>(n);

    sockaddr_storage peerAddr;
    int addrlen = sizeof(peerAddr);
    sockaddr* addr = reinterpret_cast<sockaddr*>(&peerAddr);

    int len = recvfrom( _fd, buffer, buflen, 0, 
                        addr, &addrlen );

    if( len == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
    {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(_fd, &fds);
    
        this->waitSelect(&fds, 0, 0, _timeout);

        len = recvfrom( _fd, buffer, buflen, 0,
                        addr,  &addrlen );

        if(len < 0)
            throw System::IOError("recvfrom");
    }

    _peerAddr.impl()->init(addr, addrlen);

    return len;
}


std::size_t UdpSocketImpl::beginRead(System::EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    if(_ioh.handle() == INVALID_HANDLE_VALUE)
    {
        loop.selector().enableOverlapped(_ioh);
    }

    assert(buffer != 0);
    _eventFlags |= FD_READ;

    ULONG maxLen = std::numeric_limits<ULONG>::max();
    _receiveBuffer.buf = buffer;
    _receiveBuffer.len = n > maxLen ? maxLen : static_cast<ULONG>(n);

    setEventFlags(_ioh.handle(), _eventFlags);
    return 0;
}


std::size_t UdpSocketImpl::endRead(System::EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    _eventFlags &= ~FD_READ;
    setEventFlags(_ioh.handle(), _eventFlags);

    sockaddr_storage peerAddr;
    int addrlen = sizeof(peerAddr);
    sockaddr* addr = reinterpret_cast<sockaddr*>(&peerAddr);

    int len = recvfrom( _fd, _receiveBuffer.buf, _receiveBuffer.len, 0,
                        addr, &addrlen );

    if( len == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
    {
        //Set socket to blocking mode
        setEventFlags(0,0);

        u_long argp = 0;
        ::ioctlsocket(_fd, FIONBIO, &argp);

        len = recvfrom( _fd, _receiveBuffer.buf, _receiveBuffer.len, 0,
                        addr,  &addrlen );

        //Set socket to non-blocking mode
        argp = 1;
        ::ioctlsocket(_fd, FIONBIO, &argp);

        setEventFlags(_ioh.handle(), _eventFlags);

        if(len < 0)
            throw System::IOError("recvfrom");
    }

    _peerAddr.impl()->init(addr, addrlen);

    return len;
}


std::size_t UdpSocketImpl::write(const char* buffer, std::size_t n)
{
    unsigned int maxLen = std::numeric_limits<int>::max();
    int buflen = n > maxLen ? static_cast<int>(maxLen) : static_cast<int>(n);

    int addrlen = sizeof(_sendAddr);
    int len = sendto( _fd, buffer, buflen, 
                      0, (sockaddr*)&_sendAddr, addrlen );

    if( len == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
    {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(_fd, &fds);
    
        this->waitSelect(0, &fds, 0, _timeout);

        len = sendto( _fd, buffer, buflen, 0,
                        (sockaddr*) &_sendAddr, addrlen );

        if(len < 0)
            throw System::IOError("sendto");
    }

    return len;
}



std::size_t UdpSocketImpl::beginWrite(System::EventLoop& loop, const char* buffer, std::size_t n)
{
    if(_ioh.handle() == INVALID_HANDLE_VALUE)
    {
        loop.selector().enableOverlapped(_ioh);
    }

    ULONG maxLen = std::numeric_limits<ULONG>::max();
    _sendBuffer.buf = const_cast<char*>(buffer);
    _sendBuffer.len = n > maxLen ? maxLen : static_cast<ULONG>(n);

    DWORD numberOfBytesSent = 0;

    int rc = WSASendTo( _fd, &_sendBuffer, 1, &numberOfBytesSent, 0,
                        (sockaddr*)&_sendAddr, sizeof(_sendAddr), NULL, NULL);

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


std::size_t UdpSocketImpl::endWrite(System::EventLoop& loop, const char* buffer, std::size_t n)
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
    int rc = WSASendTo(_fd, &_sendBuffer, 1, &bytesSend, 0, 
                       (sockaddr*)&_sendAddr, sizeof(_sendAddr), NULL, NULL);

    //Set socket to non-blocking mode
    argp = 1;
    ::ioctlsocket(_fd, FIONBIO, &argp);
    setEventFlags(_ioh.handle(), _eventFlags);

    if(rc == SOCKET_ERROR)
        throw System::IOError("WSASend");

    return  bytesSend;
}


int UdpSocketImpl::waitSelect(fd_set* rfds, fd_set* wfds, fd_set* efds, std::size_t timeout)
{
    struct timeval* tval = 0;
    struct timeval tv;
    if(timeout != System::EventLoop::WaitInfinite)
    {
        std::size_t timeoutSecs = timeout / 1000;
        unsigned long maxSecs = std::numeric_limits<long>::max();

        tv.tv_sec = timeoutSecs > maxSecs ? static_cast<long>(maxSecs) 
                                          : static_cast<long>(timeoutSecs);
        
        tv.tv_usec = (timeout % 1000) * 1000;
        tval = &tv;
    }

    int ret = select(FD_SETSIZE, rfds, wfds, efds, tval);
    if(0 == ret)
        throw System::IOError("socket write timeout");
    else if(SOCKET_ERROR == ret)
        throw System::IOError("select failed");

    return ret;
}

} // namespace Net

} // namespace Pt

