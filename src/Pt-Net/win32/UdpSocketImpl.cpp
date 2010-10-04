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
#include <Pt/Net/AddrInfo.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/IOError.h>
#include <limits>
#include <cassert>

namespace Pt {

namespace Net {

UdpSocketImpl::UdpSocketImpl()
: _connectResult(0)
, _fd(INVALID_SOCKET)
, _isConnected(false)
, _isBound(false)
, _eventFlags(FD_CLOSE)
, _waitEvent( WSACreateEvent() )
, _currentEventHandle(INVALID_HANDLE_VALUE)
, _timeout(Pt::System::EventLoop::WaitInfinite)
, _dataSends(0)
{
    _currentEventHandle = _waitEvent;
}


UdpSocketImpl::~UdpSocketImpl()
{
}


void UdpSocketImpl::close()
{
    if( _fd == INVALID_SOCKET )
        return;

    _eventFlags  = 0;
    setEventFlags(0, 0); // is this needed ?
    ::closesocket(_fd);
    _fd = INVALID_SOCKET;
    _isConnected = false;
    _isBound = false;
}


void UdpSocketImpl::bind(const std::string& ipaddr, unsigned short int port, unsigned flags)
{
    AddrInfo ai(ipaddr, port, true);

    static const int on = 1;

    for (AddrInfoImpl::const_iterator it = ai.impl()->begin(); it != ai.impl()->end(); ++it)
    {
        if( _isConnected )
        {
            if(it->ai_family != _peeraddr.sa_family)
                continue;
        }
        else if( _isBound )
        {
            if(it->ai_family != _servaddr.sa_family)
                this->close();
        }

        if( _fd == INVALID_SOCKET )
            _fd = WSASocket(_addrInfoPtr->ai_family, SOCK_DGRAM, 0, NULL, 0, 0);

        if( _fd == INVALID_SOCKET )
            continue;

#if defined(IPPROTO_IPV6) && defined(IPV6_V6ONLY)

        if( it->ai_family == AF_INET6 )
        {
            if( ::setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) < 0 )
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

        if( ! _isConnected )
            this->close();
    }

    if( WSAGetLastError() == WSAEADDRINUSE )
        throw AddressInUse();
    else
        throw System::SystemError("bind");
}


void UdpSocketImpl::connect(const AddrInfo& ai)
{
    _addrInfo = ai;
    _addrInfoPtr = _addrInfo.impl()->begin();

    for( ; _addrInfoPtr != _addrInfo.impl()->end(); ++_addrInfoPtr)
    {
        if( _isBound )
        {
            if(_addrInfoPtr->ai_family != _servaddr.sa_family)
                continue;
        }
        else if( _isConnected )
        {
            if(_addrInfoPtr->ai_family != _peeraddr.sa_family)
                this->close();
        }

        if( _fd == INVALID_SOCKET )
            _fd = WSASocket(_addrInfoPtr->ai_family, SOCK_DGRAM, 0, NULL, 0, 0);

        if( _fd == INVALID_SOCKET )
            continue;

        std::memmove(&_peeraddr, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen);

        if( 0 == ::connect(_fd, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) )
        {
            _isConnected = true;
            return;
        }

        if( ! _isBound )
            this->close();
    }

    throw System::IOError("connect failed");

    //this->beginConnect(addrinfo);
    //this->endConnect();
}


bool UdpSocketImpl::beginConnect(const AddrInfo& ai)
{
    assert( ! _isConnected );

    _addrInfo = ai;
    _addrInfoPtr = _addrInfo.impl()->begin();
    _connectResult = tryConnect();
    checkPendingError();

    return _isConnected;
}


const char* UdpSocketImpl::tryConnect()
{
    if( _addrInfoPtr == _addrInfo.impl()->end() )
    {
        return "invalid address";
    }

    while (true)
    {
        while (true)
        {
            _fd = WSASocket(_addrInfoPtr->ai_family, SOCK_DGRAM, 0, NULL, 0, 0);

             if (_fd != INVALID_SOCKET)
                break;

             if (++_addrInfoPtr == _addrInfo.impl()->end())
                return "WSASocket failed";
        }

        std::memmove(&_peeraddr, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen);

        if( ::connect(_fd, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0 )
        {
            _isConnected = true;
            break;
        }

        if (errno == WSAEINPROGRESS)
        {
            break;
        }

        close();

        if (++_addrInfoPtr == _addrInfo.impl()->end())
            return "connect failed";
    }

    return 0;
}


void UdpSocketImpl::checkPendingError()
{
    if(_connectResult)
    {
        const char* p = _connectResult;
        _connectResult = 0;
        throw System::IOError(p);
    }
}


void UdpSocketImpl::endConnect()
{
    _eventFlags &= ~FD_CONNECT;
    this->setEventFlags(_currentEventHandle, _eventFlags);

    checkPendingError();

    if( _isConnected )
        return;

    _eventFlags |= FD_CONNECT;
    this->setEventFlags(_currentEventHandle, _eventFlags);

    try
    {
        while (true)
        {
            bool avail = this->wait(_timeout);

            if(avail)
            {
                int sockerr = 0;
                socklen_t optlen = sizeof(sockerr);

                if( ::getsockopt(_fd, SOL_SOCKET, SO_ERROR, (char*)&sockerr, &optlen) != 0 )
                {
                    close();
                    throw System::SystemError("getsockopt");
                }

                if( sockerr == 0 )
                {
                    _isConnected = true;
                    _eventFlags &= ~FD_CONNECT;
                    this->setEventFlags(_currentEventHandle, _eventFlags);
                    return;
                }

                if (++_addrInfoPtr == _addrInfo.impl()->end())
                {
                    // no more addrInfo - propagate error
                    throw System::IOError("connect failed");
                }
            }
            else if (++_addrInfoPtr == _addrInfo.impl()->end())
            {
                throw System::IOTimeout();
            }

            close();
            _connectResult = tryConnect();

            if( _isConnected )
            {
                _eventFlags &= ~FD_CONNECT;
                this->setEventFlags(_currentEventHandle, _eventFlags);
                return;
            }

            checkPendingError();
        }
    }
    catch(...)
    {
        close();
        throw;
    }
}


bool UdpSocketImpl::setWaitHandle(HANDLE h, bool& avail)
{
    avail = _dataSends != 0;

    if( _currentEventHandle == h)
        return true;

    _currentEventHandle = h;

    this->setEventFlags(_currentEventHandle, _eventFlags);
    return true;
}


bool UdpSocketImpl::wait(std::size_t umsecs)
{
    DWORD msecs = umsecs;
    if(umsecs == Pt::System::EventLoop::WaitInfinite)
    {
        msecs = INFINITE;
    }
    else if( umsecs > static_cast<size_t>(std::numeric_limits<DWORD>::max()) )
    {
        msecs = std::numeric_limits<int>::max();
    }

    // why dataSends ?
    if( _dataSends != 0 ||
        WSAWaitForMultipleEvents(1, &_currentEventHandle, FALSE, msecs, FALSE) != WSA_WAIT_TIMEOUT)
    {
        this->checkEvent();
        return true;
    }

    return false;
}


void UdpSocketImpl::setEventFlags(HANDLE ev, long events)
{
    if( WSAEventSelect(_fd, ev, events) == SOCKET_ERROR )
    {
        throw System::SystemError("WSAEventSelectt failed");
    }
}



size_t UdpSocketImpl::beginRead(char* buffer, size_t n, bool& eof)
{
    assert(buffer != 0);
    _eventFlags |= FD_READ;

    _receiveBuffer.buf = buffer;
    _receiveBuffer.len = n;

    setEventFlags(_currentEventHandle, _eventFlags);
    return 0;
}


size_t UdpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
    return 0;
}


size_t UdpSocketImpl::endRead(bool& eof)
{
    _eventFlags &= ~FD_READ;
   
    int len = ::recv(_fd, _receiveBuffer.buf, _receiveBuffer.len, 0);

    if( len == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
    {
        //Set socket to blocking mode
        setEventFlags(0,0);

        u_long argp = 0;
        ::ioctlsocket(_fd, FIONBIO, &argp);

        len = ::recv(_fd, _receiveBuffer.buf, _receiveBuffer.len, 0);

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

size_t UdpSocketImpl::beginWrite(const char* buffer, size_t n)
{
     _sendBuffer.buf = const_cast<char*>(buffer);
    _sendBuffer.len = n;

    DWORD numberOfBytesSent = 0;

    int rc = WSASend(_fd, &_sendBuffer, 1, &numberOfBytesSent, 0, NULL, NULL);

    if(rc == SOCKET_ERROR)
    {
        if(WSAGetLastError() == WSAEWOULDBLOCK)
        {
            _dataSends = 0;
            _eventFlags |= FD_WRITE;
            setEventFlags(_currentEventHandle, _eventFlags);
            return 0;
        }
    }

    _dataSends = numberOfBytesSent;
    //SetEvent(_currentEventHandle);
    return numberOfBytesSent;
}


size_t UdpSocketImpl::endWrite()
{
    
    if(_dataSends != 0)
    {
        size_t n =  _dataSends;
        _dataSends = 0;
        return n;
    }

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


bool UdpSocketImpl::checkEvent()
{
    ///DestructionSentry sentry(_sentry);

    if(_dataSends != 0 )
    {
       ///_socket.outputReady.send(_socket);
       return true;
    }

    WSANETWORKEVENTS events;

    if(WSAEnumNetworkEvents(_fd,_currentEventHandle, &events) == SOCKET_ERROR)
        throw System::SystemError("WSAEnumNetworkEvents failed");

    bool ev = false;

    if((events.lNetworkEvents & FD_WRITE) == FD_WRITE)
    {
       ev = true;
       ///_socket.outputReady.send(_socket);

       ///if( ! _sentry )
       ///    return ev;
    }

    if((events.lNetworkEvents & FD_READ) == FD_READ)
    {
        ev = true;
        ///_socket.inputReady.send(_socket);

        ///if( ! _sentry )
        ///   return ev;
    }

    if((events.lNetworkEvents & FD_CLOSE) == FD_CLOSE)
    {
         ev = true;
       _isConnected = false;

       ///if( ! _sentry )
       ///    return ev;
    }

    return ev;
}

} // namespace Net

} // namespace Pt
