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
#include "AddrInfo.h"
#include "EndpointImpl.h"
#include "TcpServerImpl.h"
#include "MainLoopImpl.h"
#include <Pt/Net/Endpoint.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/Logger.h>
#include <Pt/System/SystemError.h>
#include <limits>
#include <cstring>
#include <cassert>

log_define("Pt.Net.TcpSocket");

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: _errorPending(false)
, _fd(INVALID_SOCKET)
, _fdClose(false)
, _eventFlags(FD_CLOSE)
, _timeout(System::EventLoop::WaitInfinite)
, _ioh(socket)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
}


void TcpSocketImpl::setEventFlags(HANDLE ev, long events)
{
    if (WSAEventSelect(_fd, ev, events) == SOCKET_ERROR)
    {
        log_warn( "WSAEventSelect failed: " << WSAGetLastError() );
        throw System::SystemError( PT_ERROR_MSG("attach event to socket failed") );
    }
}


void TcpSocketImpl::cancel(System::EventLoop& loop)
{
    _errorPending = false;

    if(_ioh.handle() != INVALID_HANDLE_VALUE)
    {
        log_debug("cancelling io handle " << _ioh.handle());
        loop.selector().disableOverlapped(_ioh);
    }

    _eventFlags = FD_CLOSE;
    if( _fd != INVALID_SOCKET )
    {
        log_debug("cancelling socket " << _fd);
        this->setEventFlags(0, 0);
    }
}


void TcpSocketImpl::close()
{
    if( _fd == INVALID_SOCKET )
        return;

    log_debug("close socket " << _fd);
    ::closesocket(_fd);
    _fd = INVALID_SOCKET;
    _fdClose = false;
    _errorPending = false;
}


void TcpSocketImpl::accept(TcpServer& server, const TcpSocketOptions&)
{
    _fd = server.impl().accept();
    log_debug("accepted " << _fd);
}


void TcpSocketImpl::connect(const Endpoint& ep, const TcpSocketOptions&)
{
    log_debug("connect");

    _addrInfo.resolve( ep );
    _addrInfoPtr = _addrInfo.begin();

    this->connect();
}


void TcpSocketImpl::connect()
{
    for( ; ; ++_addrInfoPtr)
    {
        if(_addrInfoPtr == _addrInfo.end())
        {
            log_debug("no more address informations");
            throw System::AccessFailed( _addrInfo.host() );
        }
        
        _fd = WSASocket(_addrInfoPtr->ai_family, SOCK_STREAM, 0, NULL, 0, 0);
        if (_fd < 0)
        {
            log_debug("failed to create socket for address");
            continue;
        }
        
        //Set socket to bloking mode
        u_long argp = 0;
        ::ioctlsocket(_fd, FIONBIO, &argp);
        log_debug("created socket " << _fd);
        
        socklen_t addrlen = static_cast<socklen_t>(_addrInfoPtr->ai_addrlen);

        if( ::connect(_fd, _addrInfoPtr->ai_addr, addrlen) == 0 )
        {       
            //Set socket to non-blocking mode
            argp = 1;
            ::ioctlsocket(_fd, FIONBIO, &argp);
            break;
        }

        close();
    }
}


bool TcpSocketImpl::beginConnect(System::EventLoop& loop, const Endpoint& ep, const TcpSocketOptions&)
{
    log_debug("begin connect");

    _errorPending = false;

    if(_ioh.handle() == INVALID_HANDLE_VALUE)
    {
        loop.selector().enableOverlapped(_ioh);
        log_debug("enabled i/o handle " << _ioh.handle());
    }
    
    _addrInfo.resolve(ep);
    _addrInfoPtr = _addrInfo.begin();

    return beginConnect();
}


bool TcpSocketImpl::beginConnect()
{
    for( ; ; ++_addrInfoPtr)
    {
        if(_addrInfoPtr == _addrInfo.end())
        {
            log_debug("connect failed to all possible addresses");
            throw System::AccessFailed( _addrInfo.host() );
        }

        _fd = WSASocket(_addrInfoPtr->ai_family, SOCK_STREAM, 0, NULL, 0, 0);
        if (_fd == INVALID_SOCKET)
            continue;
    
        log_debug("created socket " << _fd);

        socklen_t addrlen = static_cast<socklen_t>(_addrInfoPtr->ai_addrlen);

        if( ::connect(_fd, _addrInfoPtr->ai_addr, addrlen) == 0 )
        {
            log_debug("immediate connect");
            return true;
        }
    
        DWORD lastError = WSAGetLastError();
        if( lastError == WSAEWOULDBLOCK || lastError == WSAEINPROGRESS )
        {
            _eventFlags |= FD_CONNECT;
            setEventFlags(_ioh.handle(), _eventFlags);
            log_debug("connect in progress");
            return false;
        }
    
        log_debug("connect failed, try next address");
        close();
    }
}


void TcpSocketImpl::endConnect(System::EventLoop& loop)
{
    log_debug("endConnect on " << _fd);

    _eventFlags &= ~FD_CONNECT;
    setEventFlags(_ioh.handle(), _eventFlags);

    if(_errorPending)
        throw System::AccessFailed(_addrInfo.host() );

    log_info("async connect not yet ready socket=" << _fd);

    _eventFlags |= FD_CONNECT;
    setEventFlags(_ioh.handle(), _eventFlags);

    bool avail = this->wait(_timeout);

    _eventFlags &= ~FD_CONNECT;
    setEventFlags(_ioh.handle(), _eventFlags);

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
                return;
            }
        }
        else
        {
            log_debug("received unknown network event " << _fd);
        }
    }
    
    log_debug("failed to connect, try next address " << _fd);
    this->close();

    ++_addrInfoPtr;
    this->connect();
}


bool TcpSocketImpl::runConnect(System::EventLoop& loop, bool& isConnected)
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
        return true;
    }

    log_debug("closing socket to try next address");

    this->close();

    try 
    {
        ++_addrInfoPtr;
        isConnected = beginConnect();
    }
    catch(const System::IOError&)
    {
        _errorPending = true;
    }

    return isConnected || _errorPending;
}


bool TcpSocketImpl::runRead(System::EventLoop& loop)
{
    WSANETWORKEVENTS events;

    if( WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR )
        throw System::SystemError("WSAEnumNetworkEvents failed");

    if( (events.lNetworkEvents & FD_CLOSE) == FD_CLOSE )
    {
        _fdClose = true;
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
        _fdClose = true;
        return true;
    }

    if( (events.lNetworkEvents & FD_WRITE) == FD_WRITE )
    {
        return true;
    }

    return false;
}


bool TcpSocketImpl::wait(std::size_t msecs)
{
    log_debug(_fd << " wait " << msecs);

    DWORD maxTimeout = std::numeric_limits<DWORD>::max() - 1;
            
    DWORD timeout = (msecs == System::EventLoop::WaitInfinite) ? INFINITE
                      : (msecs > maxTimeout) ? maxTimeout 
                          : static_cast<DWORD>(msecs);

    HANDLE h = _ioh.handle();
    if( WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1, &h, FALSE, timeout, FALSE) )
    {      
        return true;
    }

    return false;
}


void TcpSocketImpl::localEndpoint(Endpoint& ep) const
{
    sockaddr_storage sockadr;
    int l = sizeof(sockadr);
    int ret = getsockname(_fd, (sockaddr*)&sockadr, &l);

    if(ret == 0)
        ep.impl()->init( (sockaddr*)&sockadr, l );
    else
        ep.clear();
}


void TcpSocketImpl::remoteEndpoint(Endpoint& ep) const
{
    sockaddr_storage sockadr;
    int l = sizeof(sockadr);
    int ret = getpeername(_fd, (sockaddr*)&sockadr, &l);

    if(ret == 0)
        ep.impl()->init( (sockaddr*)&sockadr, l );
    else
        ep.clear();
}


std::size_t TcpSocketImpl::beginRead(System::EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    log_debug(_fd << " beginRead");

    if(_fdClose)
    {
        log_debug("EOF because of previous FD_CLOSE");
        eof = true;
        return 0;
    }

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


std::size_t TcpSocketImpl::endRead(System::EventLoop& loop, char* buffer, std::size_t, bool& eof)
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
            setEventFlags(0, 0);
        
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

    setEventFlags(_ioh.handle(), _eventFlags);

    return len;
}


std::size_t TcpSocketImpl::read(char* buf, std::size_t n, bool& eof)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_fd, &fds);

    this->waitSelect(&fds, 0, 0, _timeout);

    unsigned int maxLen = std::numeric_limits<int>::max();
    int bufsize = n > maxLen ? static_cast<int>(maxLen) : static_cast<int>(n);

    int len = ::recv(_fd, buf, bufsize, 0);
    if( len == 0)
        eof = true;
    else if(len < 0)
        throw System::IOError("recv");

    return static_cast<std::size_t>(len);
}


std::size_t TcpSocketImpl::beginWrite(System::EventLoop& loop, const char* buffer, std::size_t n)
{
    log_debug(_fd << " beginWrite");

    if(_ioh.handle() == INVALID_HANDLE_VALUE)
    {
        loop.selector().enableOverlapped(_ioh);
    }

    ULONG maxLen = std::numeric_limits<ULONG>::max();
    _sendBuffer.buf = const_cast<char*>(buffer);
    _sendBuffer.len = n > maxLen ? maxLen : static_cast<ULONG>(n);

    log_debug("previous FD_CLOSE:" << _fdClose);

    DWORD numberOfBytesSent = 0;
    int rc = WSASend(_fd, &_sendBuffer, 1, &numberOfBytesSent, 0, NULL, NULL);

    if(rc == SOCKET_ERROR)
    {
        log_debug("socket error on " << _fd);
        if(WSAGetLastError() == WSAEWOULDBLOCK)
        {
            log_debug("WSAEWOULDBLOCK on " << _fd);
            _eventFlags |= FD_WRITE;
            setEventFlags(_ioh.handle(), _eventFlags);
            return 0;
        }
        else
        {
            std::cerr << WSAGetLastError() << std::endl;
            throw System::IOError("WSASend");
        }
    }

    log_debug(_fd << " beginWrite sent " << numberOfBytesSent << " of " << n << " bytes");

    return numberOfBytesSent;
}


std::size_t TcpSocketImpl::endWrite(System::EventLoop& loop, const char* buffer, std::size_t n)
{
    log_debug(_fd << " endWrite");

    _eventFlags &= ~FD_WRITE;

    //Set socket to blocking mode
    setEventFlags(0, 0);

    u_long argp = 0;
    ::ioctlsocket(_fd, FIONBIO, &argp);

    DWORD numberOfBytesSent = 0;

    int rc = WSASend(_fd, &_sendBuffer, 1, &numberOfBytesSent, 0, NULL, NULL);

    //Set socket to non-blocking mode
    argp = 1;
    ::ioctlsocket(_fd, FIONBIO, &argp);
    setEventFlags(_ioh.handle(), _eventFlags);

    if(rc == SOCKET_ERROR)
        throw System::IOError("WSASend");

    return  numberOfBytesSent;
}


std::size_t TcpSocketImpl::write(const char* buffer, std::size_t n)
{
    log_debug(_fd << " write");

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_fd, &fds);

    this->waitSelect(0, &fds, 0, _timeout);

    ULONG maxLen = std::numeric_limits<ULONG>::max();
    _sendBuffer.buf = const_cast<char*>(buffer);
    _sendBuffer.len = n > maxLen ? maxLen : static_cast<ULONG>(n);

    DWORD numberOfBytesSent = 0;
    int rc = WSASend(_fd, &_sendBuffer, 1, &numberOfBytesSent, 0, NULL, NULL);
    if(rc == SOCKET_ERROR)
        throw System::IOError("WSASend");

    return numberOfBytesSent;
}


int TcpSocketImpl::waitSelect(fd_set* rfds, fd_set* wfds, fd_set* efds, std::size_t timeout)
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

