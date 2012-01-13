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
#include <Pt/System/EventLoop.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/IOError.h>
#include <limits>
#include <cstring>
#include <cassert>

namespace Pt {

namespace Net {

UdpSocketImpl::UdpSocketImpl(UdpSocket& socket)
: _socket(socket)
, _broadcast(false)
, _sentry(0)
//, _connectResult(0)
, _fd(INVALID_SOCKET)
, _isConnected(false)
, _isBound(false)
, _eventFlags(FD_CLOSE)
, _currentEventHandle(INVALID_HANDLE_VALUE)
, _timeout(Pt::System::EventLoop::WaitInfinite)
{
}


UdpSocketImpl::~UdpSocketImpl()
{
    if(_sentry)
        _sentry->detach();
}


void UdpSocketImpl::setEventFlags(HANDLE ev, long events)
{
    if( WSAEventSelect(_fd, ev, events) == SOCKET_ERROR )
    {
        throw System::SystemError("WSAEventSelectt failed");
    }
}


void UdpSocketImpl::attach(System::EventLoop& loop)
{
    /*if( _fd == INVALID_SOCKET)
        return;

    HANDLE h = loop.impl().enable(_socket);
    _currentEventHandle = h;
    this->setEventFlags(_currentEventHandle, _eventFlags);*/
}


void UdpSocketImpl::detach(System::EventLoop& loop)
{
    /*if( _fd != INVALID_SOCKET)
        setEventFlags(_waitEvent, _eventFlags);

    loop.impl().disable(_socket);*/

    //if( _fd != INVALID_SOCKET)
    //    setEventFlags(_waitEvent, _eventFlags);
}


void UdpSocketImpl::cancel(System::EventLoop& loop)
{
    if(_currentEventHandle != INVALID_HANDLE_VALUE)
    {
        loop.impl().disable(_socket);
        _currentEventHandle = INVALID_HANDLE_VALUE;
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

    /*_eventFlags  = 0;
    setEventFlags(0, 0); // is this needed ?

    if(loop)
        this->detach(*loop);*/

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
                this->close(loop);
                throw System::SystemError("setsockopt IPV6_V6ONLY failed");
            }
        }
#endif

        if( ::bind(_fd, it->ai_addr, it->ai_addrlen) == 0 )
        {
            _isBound = true;
            std::memmove(&_servaddr, it->ai_addr, it->ai_addrlen);

            /*if(loop)
                this->attach(*loop);*/

            return;
        }

        addrInUse = WSAGetLastError() == WSAEADDRINUSE;

        if( ! _isConnected )
            this->close();
    }

    if(addrInUse)
        throw AddressInUse();
    else
        throw System::IOError("bind");
}


void UdpSocketImpl::connect(const AddrInfo& ai)
{
    _addrInfo = ai;
    _addrInfoPtr = _addrInfo.impl()->begin();

    for( ; _addrInfoPtr != _addrInfo.impl()->end(); ++_addrInfoPtr)
    {
        if( _isBound )
        {
            if(_addrInfoPtr->ai_family != _servaddr.ss_family)
                continue;
        }
        else if( _isConnected )
        {
            if(_addrInfoPtr->ai_family != _peeraddr.ss_family)
                this->close();
        }

        if( _fd == INVALID_SOCKET )
            _fd = WSASocket(_addrInfoPtr->ai_family, SOCK_DGRAM, 0, NULL, 0, 0);

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

        std::memmove(&_peeraddr, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen);

        if( 0 == ::connect(_fd, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) )
        {
            _isConnected = true;

            /*if(loop)
                this->attach(*loop);*/

            return;
        }

        if( ! _isBound )
            this->close();
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


std::string UdpSocketImpl::getSockAddr() const
{
    const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&_servaddr);

    // SOCKADDR* saddr = const_cast<SOCKADDR*>(&_servaddr);
    // DWORD len = 32;
    // TCHAR adr[32];
    // WSAAddressToString(saddr, sizeof(SOCKADDR), NULL, adr, &len);

    // std::string address;
    // address.reserve(32);
    // for(unsigned n = 0; n < len; n++)
    // {
    //     address.push_back( int(adr[n]) );
    // }

    return inet_ntoa(sa->sin_addr);
    //return adr;
}


std::string UdpSocketImpl::getPeerAddr() const
{
    const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&_peeraddr);

    //char adr[15]; //TODO: Windows CE wchar_t
    //WSAAddressToString(sa, sizeof(sa), NULL, adr, 15);

    return inet_ntoa(sa->sin_addr);
    //return adr;
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


/*bool UdpSocketImpl::run(System::EventLoop& loop)
{
    DestructionSentry sentry(_sentry);

    if(_dataSends != 0 )
    {
       _socket.outputReady().send(_socket);
       return true;
    }

    WSANETWORKEVENTS events;

    if(WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR)
        throw System::SystemError("WSAEnumNetworkEvents failed");

    bool ev = false;

    if(_socket.wavail() || ((events.lNetworkEvents & FD_WRITE) == FD_WRITE) )
    {
       ev = true;
       _socket.outputReady().send(_socket);

       if( ! _sentry )
           return ev;
    }

    if(_socket.ravail() || ((events.lNetworkEvents & FD_READ) == FD_READ) )
    {
        ev = true;
        _socket.inputReady().send(_socket);

        if( ! _sentry )
           return ev;
    }

    return ev;
}*/


/*bool UdpSocketImpl::setWaitHandle(HANDLE h, bool& avail)
{
    avail = _dataSends != 0;

    if( _currentEventHandle == h)
        return true;

    _currentEventHandle = h;

    this->setEventFlags(_currentEventHandle, _eventFlags);
    return true;
}*/


size_t UdpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
    return 0;
}


size_t UdpSocketImpl::beginRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    if(_currentEventHandle == INVALID_HANDLE_VALUE)
    {
        HANDLE h = loop.impl().enable(_socket);
        _currentEventHandle = h;
    }

    assert(buffer != 0);
    _eventFlags |= FD_READ;

    _receiveBuffer.buf = buffer;
    _receiveBuffer.len = n;

    setEventFlags(_currentEventHandle, _eventFlags);
    return 0;
}


size_t UdpSocketImpl::endRead(System::EventLoop& loop, bool& eof)
{
    _eventFlags &= ~FD_READ;

    int addrlen = sizeof(_peeraddr);
    //int len = ::recv(_fd, _receiveBuffer.buf, _receiveBuffer.len, 0);
    int len = recvfrom( _fd, _receiveBuffer.buf, _receiveBuffer.len, 0,
                        (sockaddr*) &_peeraddr, &addrlen );

    if( len == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
    {
        //Set socket to blocking mode
        setEventFlags(0,0);

        u_long argp = 0;
        ::ioctlsocket(_fd, FIONBIO, &argp);

        //len = ::recv(_fd, _receiveBuffer.buf, _receiveBuffer.len, 0);
        len = recvfrom( _fd, _receiveBuffer.buf, _receiveBuffer.len, 0,
                        (sockaddr*) &_peeraddr,  &addrlen );
        //Set socket to non-blocking mode
        argp = 1;
        ::ioctlsocket(_fd, FIONBIO, &argp);
    }

    setEventFlags(_currentEventHandle, _eventFlags);

    return len;
}


size_t UdpSocketImpl::write(const char* buffer, size_t n)
{
    return 0;
}


size_t UdpSocketImpl::beginWrite(System::EventLoop& loop, const char* buffer, size_t n)
{
    if(_currentEventHandle == INVALID_HANDLE_VALUE)
    {
        HANDLE h = loop.impl().enable(_socket);
        _currentEventHandle = h;
    }

     _sendBuffer.buf = const_cast<char*>(buffer);
    _sendBuffer.len = n;

    DWORD numberOfBytesSent = 0;

    //int rc = WSASend(_fd, &_sendBuffer, 1, &numberOfBytesSent, 0, NULL, NULL);
    int rc = WSASendTo( _fd, &_sendBuffer, 1, &numberOfBytesSent, 0,
                        (sockaddr*)&_peeraddr, sizeof(_peeraddr), NULL, NULL);

    if(rc == SOCKET_ERROR)
    {
        if(WSAGetLastError() == WSAEWOULDBLOCK)
        {
            _eventFlags |= FD_WRITE;
            setEventFlags(_currentEventHandle, _eventFlags);
            return 0;
        }
    }

    //SetEvent(_currentEventHandle);
    return numberOfBytesSent;
}


size_t UdpSocketImpl::endWrite(System::EventLoop& loop)
{
    _eventFlags &= ~FD_WRITE;

    //Set socket to blocking mode
    setEventFlags(0, 0);

    u_long argp = 0;
    ::ioctlsocket(_fd, FIONBIO, &argp);

    DWORD numberOfBytesSent = 0;

    int rc = WSASend(_fd, &_sendBuffer, 1, &numberOfBytesSent, 0, NULL, NULL);

    if(rc == SOCKET_ERROR)
        throw System::SystemError( PT_ERROR_MSG("beginWrite failed") );

    //Set socket to non-blocking mode
    argp = 1;
    ::ioctlsocket(_fd, FIONBIO, &argp);
    setEventFlags(_currentEventHandle, _eventFlags);

    return  numberOfBytesSent;
}


/*bool UdpSocketImpl::checkEvent()
{
    DestructionSentry sentry(_sentry);

    if(_dataSends != 0 )
    {
       _socket.outputReady.send(_socket);
       return true;
    }

    WSANETWORKEVENTS events;

    if(WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR)
        throw System::SystemError("WSAEnumNetworkEvents failed");

    bool ev = false;

    if((events.lNetworkEvents & FD_WRITE) == FD_WRITE)
    {
       ev = true;
       _socket.outputReady.send(_socket);

       if( ! _sentry )
           return ev;
    }

    if((events.lNetworkEvents & FD_READ) == FD_READ)
    {
        ev = true;
        _socket.inputReady.send(_socket);

        if( ! _sentry )
           return ev;
    }

    // if((events.lNetworkEvents & FD_CLOSE) == FD_CLOSE)
    // {
    //      ev = true;
    //    _isConnected = false;

    //    if( ! _sentry )
    //        return ev;
    // }

    return ev;
}*/


// bool UdpSocketImpl::wait(std::size_t umsecs)
// {
//     DWORD msecs = umsecs;
//     if(umsecs == Pt::System::EventLoop::WaitInfinite)
//     {
//         msecs = INFINITE;
//     }
//     else if( umsecs > static_cast<size_t>(std::numeric_limits<DWORD>::max()) )
//     {
//         msecs = std::numeric_limits<int>::max();
//     }

//     // why dataSends ?
//     if( _dataSends != 0 ||
//         WSAWaitForMultipleEvents(1, &_currentEventHandle, FALSE, msecs, FALSE) != WSA_WAIT_TIMEOUT)
//     {
//         this->checkEvent();
//         return true;
//     }

//     return false;
// }


// bool UdpSocketImpl::beginConnect(const AddrInfo& ai)
// {
//     assert( ! _isConnected );

//     _addrInfo = ai;
//     _addrInfoPtr = _addrInfo.impl()->begin();
//     _connectResult = tryConnect();
//     checkPendingError();

//     return _isConnected;
// }


// const char* UdpSocketImpl::tryConnect()
// {
//     if( _addrInfoPtr == _addrInfo.impl()->end() )
//     {
//         return "invalid address";
//     }

//     while (true)
//     {
//         while (true)
//         {
//             _fd = WSASocket(_addrInfoPtr->ai_family, SOCK_DGRAM, 0, NULL, 0, 0);

//              if (_fd != INVALID_SOCKET)
//                 break;

//              if (++_addrInfoPtr == _addrInfo.impl()->end())
//                 return "WSASocket failed";
//         }

//         std::memmove(&_peeraddr, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen);

//         if( ::connect(_fd, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0 )
//         {
//             _isConnected = true;
//             break;
//         }

//         if (errno == WSAEINPROGRESS)
//         {
//             break;
//         }

//         close();

//         if (++_addrInfoPtr == _addrInfo.impl()->end())
//             return "connect failed";
//     }

//     return 0;
// }


// void UdpSocketImpl::checkPendingError()
// {
//     if(_connectResult)
//     {
//         const char* p = _connectResult;
//         _connectResult = 0;
//         throw System::IOError(p);
//     }
// }


// void UdpSocketImpl::endConnect()
// {
//     _eventFlags &= ~FD_CONNECT;
//     this->setEventFlags(_currentEventHandle, _eventFlags);

//     checkPendingError();

//     if( _isConnected )
//         return;

//     _eventFlags |= FD_CONNECT;
//     this->setEventFlags(_currentEventHandle, _eventFlags);

//     try
//     {
//         while (true)
//         {
//             bool avail = this->wait(_timeout);

//             if(avail)
//             {
//                 int sockerr = 0;
//                 socklen_t optlen = sizeof(sockerr);

//                 if( ::getsockopt(_fd, SOL_SOCKET, SO_ERROR, (char*)&sockerr, &optlen) != 0 )
//                 {
//                     close();
//                     throw System::SystemError("getsockopt");
//                 }

//                 if( sockerr == 0 )
//                 {
//                     _isConnected = true;
//                     _eventFlags &= ~FD_CONNECT;
//                     this->setEventFlags(_currentEventHandle, _eventFlags);
//                     return;
//                 }

//                 if (++_addrInfoPtr == _addrInfo.impl()->end())
//                 {
//                     // no more addrInfo - propagate error
//                     throw System::IOError("connect failed");
//                 }
//             }
//             else if (++_addrInfoPtr == _addrInfo.impl()->end())
//             {
//                 throw System::IOTimeout();
//             }

//             close();
//             _connectResult = tryConnect();

//             if( _isConnected )
//             {
//                 _eventFlags &= ~FD_CONNECT;
//                 this->setEventFlags(_currentEventHandle, _eventFlags);
//                 return;
//             }

//             checkPendingError();
//         }
//     }
//     catch(...)
//     {
//         close();
//         throw;
//     }
// }

} // namespace Net

} // namespace Pt
