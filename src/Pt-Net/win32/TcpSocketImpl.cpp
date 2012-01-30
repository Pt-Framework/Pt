/*
 * Copyright (C) 2009 Marc Boris Duerner, Tommi Maekitalo,
 *                    Laurentiu-Gheorghe Crisan
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
#include "TcpSocketImpl.h"
#include "AddrInfoImpl.h"
#include "TcpServerImpl.h"
#include "MainLoopImpl.h"
#include <Pt/Net/AddrInfo.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/Logger.h>
#include <Pt/System/SystemError.h>
#include <cstring>
#include <cassert>

log_define("Pt.System.TcpSocket");

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: _errorPending(false)
, _fd(INVALID_SOCKET)
, _isConnected(false)
, _eventFlags(FD_CLOSE)
, _timeout(System::EventLoop::WaitInfinite)
, _ioh(socket)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
}


void TcpSocketImpl::attachEvent(HANDLE ev, long events)
{
    if (WSAEventSelect(_fd, ev, events) == SOCKET_ERROR)
    {
        log_warn( "WSAEventSelect failed: " << WSAGetLastError() );
        throw System::SystemError( PT_ERROR_MSG("attach event to socket failed") );
    }
}


void TcpSocketImpl::cancel(System::EventLoop& loop)
{
    if(_ioh.handle() != INVALID_HANDLE_VALUE)
    {
        log_debug("cancelling io handle " << _fd);
        loop.selector().disableOverlapped(_ioh);
    }

    _eventFlags = FD_CLOSE;
    if( _fd != INVALID_SOCKET )
    {
        log_debug("cancelling socket " << _fd);
        this->attachEvent(0, 0);
    }
}


void TcpSocketImpl::close()
{
    if( _fd == INVALID_SOCKET )
        return;

    log_debug("close socket " << _fd);
    ::closesocket(_fd);
    _fd = INVALID_SOCKET;
    _isConnected = false;
}


void TcpSocketImpl::accept(const TcpServer& server, unsigned flags)
{
    _fd = server.impl().accept();
    _isConnected = true;
    log_debug("accepted " << _fd);
}


void TcpSocketImpl::connect(const AddrInfo& addrinfo)
{
    log_debug("connect");
    assert( ! _isConnected );

    _addrInfo = addrinfo;
    _addrInfoPtr = _addrInfo.impl()->begin();

    for( ; _addrInfoPtr != _addrInfo.impl()->end(); ++_addrInfoPtr)
    {
        _fd = WSASocket(_addrInfoPtr->ai_family, SOCK_STREAM, 0, NULL, 0, 0);

        if (_fd < 0)
            continue;

       //Set socket to bloking mode
       u_long argp = 0;
       ::ioctlsocket(_fd, FIONBIO, &argp);
        
        log_debug("created socket " << _fd);

        if( ::connect(_fd, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0 )
        {
            _isConnected = true;
            //Set socket to non-blocking mode
            argp = 1;
            ::ioctlsocket(_fd, FIONBIO, &argp);
            break;
        }
        close();
    }

    if(_addrInfoPtr == _addrInfo.impl()->end())
    {
        log_debug("no more address informations");
        throw System::IOError("connect failed");
    }
}


bool TcpSocketImpl::beginConnect(System::EventLoop& loop, const AddrInfo& ai)
{
    assert( ! _isConnected );
    log_debug("begin connect");

    _errorPending = false;

    if(_ioh.handle() == INVALID_HANDLE_VALUE)
    {
        loop.selector().enableOverlapped(_ioh);
    }
    
    _addrInfo = ai;
    _addrInfoPtr = _addrInfo.impl()->begin();

    while(true)
    {
        if(_addrInfoPtr == _addrInfo.impl()->end())
            throw System::IOError("connect failed");

        try
        {
            _isConnected = beginConnect(*_addrInfoPtr);
            break;
        } 
        catch(const System::IOError&)
        {
            ++_addrInfoPtr;
        }
    }

    return _isConnected;
}


bool TcpSocketImpl::beginConnect(const ::addrinfo& ai)
{
    _fd = WSASocket(ai.ai_family, SOCK_STREAM, 0, NULL, 0, 0);
    log_debug("created socket " << _fd);

    if (_fd == INVALID_SOCKET)
        throw System::IOError("WSASocket");

    if( ::connect(_fd, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0 )
    {
        _isConnected = true;
        log_debug("immediate connect");
        return true;
    }

    DWORD lastError = WSAGetLastError();
    if( lastError == WSAEWOULDBLOCK || lastError == WSAEINPROGRESS )
    {
        _eventFlags |= FD_CONNECT;
        attachEvent(_ioh.handle(), _eventFlags);
        log_debug("connect in progress");
        return false;
    }

    log_debug("connect failed");
    close();
    throw System::IOError("connect");

    return false;
}


void TcpSocketImpl::endConnect(System::EventLoop& loop)
{
    log_debug("endConnect on " << _fd);

    _eventFlags &= ~FD_CONNECT;
    attachEvent(_ioh.handle(), _eventFlags);

    if(_errorPending)
        throw System::IOError("connect");

    if( _isConnected )
        return;

    log_debug("wait for connect on " << _fd);

    try
    {
        bool hasTimeout = false;
        while (true)
        {
            _eventFlags |= FD_CONNECT;
            attachEvent(_ioh.handle(), _eventFlags);

            bool avail = this->wait(_timeout);
            if( ! avail )
                hasTimeout = true;

            _eventFlags &= ~FD_CONNECT;
            attachEvent(_ioh.handle(), _eventFlags);

            if(avail)
            {
                WSANETWORKEVENTS events;
                if( WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR )
                    throw System::SystemError("WSAEnumNetworkEvents failed");

                if( (events.lNetworkEvents & FD_CLOSE) == FD_CLOSE )
                {
                    log_debug("received close event " << _fd);
                }
                else if( (events.lNetworkEvents & FD_CONNECT) == FD_CONNECT )
                {
                    int s = FD_CONNECT_BIT;
                    if(events.iErrorCode[s] == 0)
                    {
                        log_debug("connected " << _fd);
                        _isConnected = true;
                        return;
                    }
                }
                else
                {
                    log_debug("received unknown network event " << _fd);
                    continue;
                }
            }
            
            log_debug("failed to connect, try next address " << _fd);
            this->close();
            ++_addrInfoPtr;

            while(true)
            {
                if( _addrInfoPtr == _addrInfo.impl()->end() )
                {
                    log_debug("no more addresses to try");
                    if(hasTimeout)
                        throw System::IOTimeout();
                    else
                        throw System::IOError("connect");
                }
        
                try
                {
                    log_debug("try next address");
                    _isConnected = beginConnect(*_addrInfoPtr);
                    break;
                } 
                catch(const System::IOError&)
                {
                    ++_addrInfoPtr;
                }
            }
        }
    }
    catch(...)
    {
        close();
        throw;
    }
}


bool TcpSocketImpl::runConnect(System::EventLoop& loop)
{
    WSANETWORKEVENTS events;

    if( WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR )
        throw System::SystemError("WSAEnumNetworkEvents failed");

    if( (events.lNetworkEvents & FD_CLOSE) == FD_CLOSE )
    {
        log_debug("received FD_CLOSE for connect");
       _errorPending = true;
       return true;
    }

    if( (events.lNetworkEvents & FD_CONNECT) != FD_CONNECT )
    {
        log_debug("network events did not contain FD_CONNECT for connect");
        return false;
    }

    int s = FD_CONNECT_BIT;
    if(events.iErrorCode[s] == 0)
    {
        log_debug("connect was successful");
        _isConnected = true;
        return true;
    }

    log_debug("closing socket to try next address");
    this->close();

    while( true )
    {
        if( _addrInfoPtr == _addrInfo.impl()->end() )
        {
            _errorPending = true;
            return true;
        }

        try
        {
            _isConnected = beginConnect(*_addrInfoPtr);
            break;
        } 
        catch(const System::IOError&)
        {
            ++_addrInfoPtr;
        }
    }

    return _isConnected;
}


bool TcpSocketImpl::runRead(System::EventLoop& loop)
{
    WSANETWORKEVENTS events;

    if( WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR )
        throw System::SystemError("WSAEnumNetworkEvents failed");

    if( (events.lNetworkEvents & FD_CLOSE) == FD_CLOSE )
    {
       _isConnected = false;
       return true;
    }

    if( (events.lNetworkEvents & FD_READ) == FD_READ )
    {
        return true;
    }

    return false;
}


bool TcpSocketImpl::runWrite(System::EventLoop& loop)
{
    WSANETWORKEVENTS events;

    if( WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR )
        throw System::SystemError("WSAEnumNetworkEvents failed");

    if( (events.lNetworkEvents & FD_CLOSE) == FD_CLOSE )
    {
       _isConnected = false;
       return true;
    }

    if( (events.lNetworkEvents & FD_WRITE) == FD_WRITE )
    {
       return true;
    }

    return false;
}


bool TcpSocketImpl::wait(std::size_t umsecs)
{
    log_debug(_fd << " wait " << umsecs);

    int msecs = umsecs;
    if(umsecs == Pt::System::EventLoop::WaitInfinite)
    {
        msecs = INFINITE;
    }
    else if( umsecs > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
        msecs = std::numeric_limits<int>::max();
    }

    HANDLE h = _ioh.handle();
    if( WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1, &h, FALSE, msecs, FALSE) )
    {      
        return true;
    }

    return false;
}


std::string TcpSocketImpl::socketAddress() const
{
    SOCKADDR sockadr;
    int l = sizeof(sockadr);
    int ret = getsockname(_fd, &sockadr, &l);

    SOCKADDR* saddr = const_cast<SOCKADDR*>(&sockadr);
    DWORD len = 32;
    TCHAR adr[32];
    WSAAddressToString(saddr, sizeof(SOCKADDR), NULL, adr, &len);

    std::string address;
    for(unsigned n = 0; n < len; n++)
    {
        address.push_back( int(adr[n]) );
    }

    return address;
}


std::string TcpSocketImpl::peerAddress() const
{
    SOCKADDR sockadr;
    int l = sizeof(sockadr);
    int ret = getpeername(_fd, &sockadr, &l);

    SOCKADDR* saddr = const_cast<SOCKADDR*>(&sockadr);
    DWORD len = 32;
    TCHAR adr[32];
    WSAAddressToString(saddr, sizeof(SOCKADDR), NULL, adr, &len);

    std::string address;
    for(unsigned n = 0; n < len; n++)
    {
        address.push_back( int(adr[n]) );
    }

    return address;
}


size_t TcpSocketImpl::beginRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    log_debug(_fd << " beginRead");

    if(_ioh.handle() == INVALID_HANDLE_VALUE)
    {
        loop.selector().enableOverlapped(_ioh);
    }

    assert(buffer != 0);
    _eventFlags |= FD_READ;

    _receiveBuffer.buf = buffer;
    _receiveBuffer.len = n;

    attachEvent(_ioh.handle(), _eventFlags);
    return 0;
}


size_t TcpSocketImpl::endRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    log_debug(_fd << " endRead");
    _eventFlags &= ~FD_READ;

    int len = ::recv(_fd, _receiveBuffer.buf, _receiveBuffer.len, 0);

    if( len == 0)
    {
        eof = true;
    }
    else if(len == -1)
    {
        int err = WSAGetLastError();
        if(err == WSAEWOULDBLOCK)
        {
            //Set socket to blocking mode
            attachEvent(0, 0);
        
            u_long argp = 0;
            ::ioctlsocket(_fd, FIONBIO, &argp);
        
            len = ::recv(_fd, _receiveBuffer.buf, _receiveBuffer.len, 0);
        
            if( len == 0)
                eof = true;
        
            //Set socket to non-blocking mode
            argp = 1;
            ::ioctlsocket(_fd, FIONBIO, &argp);
        }
        else if(err == WSAECONNRESET)
        {
            eof = true;
        }
    }

    attachEvent(_ioh.handle(), _eventFlags);

    return len;
}


size_t TcpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
    //Set socket to blocking mode
    u_long argp = 0;
    ::ioctlsocket(_fd, FIONBIO, &argp);

    int len = ::recv(_fd, buffer, count, 0);

    //Set socket to non-blocking mode
    argp = 1;
    ::ioctlsocket(_fd, FIONBIO, &argp);

    if( len == 0)
        eof = true;
    else if(len < 0)
        throw System::IOError("recv");

    return static_cast<size_t>(len);
}


size_t TcpSocketImpl::beginWrite(System::EventLoop& loop, const char* buffer, size_t n)
{
    log_debug(_fd << " beginWrite");

    if(_ioh.handle() == INVALID_HANDLE_VALUE)
    {
        loop.selector().enableOverlapped(_ioh);
    }

    _sendBuffer.buf = const_cast<char*>(buffer);
    _sendBuffer.len = n;

    DWORD numberOfBytesSent = 0;

    int rc = WSASend(_fd, &_sendBuffer, 1, &numberOfBytesSent, 0, NULL, NULL);

    if(rc == SOCKET_ERROR)
    {
        log_debug("socket error on " << _fd);
        if(WSAGetLastError() == WSAEWOULDBLOCK)
        {
            log_debug("WSAEWOULDBLOCK on " << _fd);
            _eventFlags |= FD_WRITE;
            attachEvent(_ioh.handle(), _eventFlags);
            return 0;
        }
        else
        {
            throw System::IOError("WSASend");
        }
    }

    log_debug(_fd << " beginWrite sent " << numberOfBytesSent << " of " << n << " bytes");

    return numberOfBytesSent;
}


size_t TcpSocketImpl::endWrite(System::EventLoop& loop, const char* buffer, size_t n)
{
    log_debug(_fd << " endWrite");

    _eventFlags &= ~FD_WRITE;

    //Set socket to blocking mode
    attachEvent(0, 0);

    u_long argp = 0;
    ::ioctlsocket(_fd, FIONBIO, &argp);

    DWORD numberOfBytesSent = 0;

    int rc = WSASend(_fd, &_sendBuffer, 1, &numberOfBytesSent, 0, NULL, NULL);

    //Set socket to non-blocking mode
    argp = 1;
    ::ioctlsocket(_fd, FIONBIO, &argp);
    attachEvent(_ioh.handle(), _eventFlags);

    if(rc == SOCKET_ERROR)
        throw System::IOError("WSASend");

    return  numberOfBytesSent;
}


size_t TcpSocketImpl::write(const char* buffer, size_t count)
{
    log_debug(_fd << " endWrite");

    _sendBuffer.buf = const_cast<char*>(buffer);
    _sendBuffer.len = count;

    //Set socket to blocking mode
    u_long argp = 0;
    ::ioctlsocket(_fd, FIONBIO, &argp);

    DWORD numberOfBytesSent = 0;

    int rc = WSASend(_fd, &_sendBuffer, 1, &numberOfBytesSent, 0, NULL, NULL);

    //Set socket to non-blocking mode
    argp = 1;
    ::ioctlsocket(_fd, FIONBIO, &argp);

    if(rc == SOCKET_ERROR)
        throw System::IOError("WSASend");

    return  numberOfBytesSent;
}

} // namespace Net

} // namespace Pt

