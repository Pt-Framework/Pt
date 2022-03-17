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

#include <Mstcpip.h>

PT_LOG_DEFINE("Pt.Net.TcpSocket");

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: _errorPending(false)
, _fd(INVALID_SOCKET)
, _timeout(System::EventLoop::WaitInfinite)
, _ioh(socket)
, _fdClose(false)
, _eventFlags(FD_CLOSE)
, _socketError(0)
, _bytesSent(0)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
}


void TcpSocketImpl::eventSelect(HANDLE ev, long events)
{
    if (WSAEventSelect(_fd, ev, events) == SOCKET_ERROR)
    {
        PT_LOG_WARN( "WSAEventSelect failed: " << WSAGetLastError() );
        throw System::SystemError("WSAEventSelect");
    }
}


void TcpSocketImpl::cancel(System::EventLoop& loop)
{
    _errorPending = false;
    _socketError = 0;
    _bytesSent = 0;

    if(_ioh.handle() != INVALID_HANDLE_VALUE)
    {
        PT_LOG_DEBUG("cancelling io handle " << _ioh.handle());
        loop.selector().disableOverlapped(_ioh);
    }

    _eventFlags = FD_CLOSE;
    if( _fd != INVALID_SOCKET )
    {
        PT_LOG_DEBUG("cancelling socket " << _fd);
        this->eventSelect(0, 0);
    }
}


void TcpSocketImpl::init()
{
    if( _opts.keepAlive() == 0 )
    {
        PT_LOG_DEBUG("using SO_KEEPALIVE");
        BOOL val = TRUE;

        int rv = ::setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof(val));
        if(rv == SOCKET_ERROR)
          throw IOError("setsockopt SO_KEEPALIVE");
    }
    else if( _opts.keepAlive() > 0 )
    {
        PT_LOG_DEBUG( "using keep-alive: " << _opts.keepAlive() );
        DWORD msecs = _opts.keepAlive() * 1000;

        struct tcp_keepalive vals;
        vals.onoff = 1;
        vals.keepalivetime = msecs;
        vals.keepaliveinterval = msecs;

        DWORD bytes_returned = 0;
        int rv = WSAIoctl(_fd, SIO_KEEPALIVE_VALS, &vals,
                          sizeof(vals), NULL, 0,
                          &bytes_returned, NULL, NULL);

        if(rv == SOCKET_ERROR)
            throw IOError("ioctl SIO_KEEPALIVE_VALS");
    }
}


void TcpSocketImpl::close()
{
    if( _fd == INVALID_SOCKET )
        return;

    PT_LOG_DEBUG("close socket " << _fd);
    ::closesocket(_fd);
    _fd = INVALID_SOCKET;
    _fdClose = false;
    _errorPending = false;
    _socketError = 0;
    _bytesSent = 0;
}


void TcpSocketImpl::accept(TcpServer& server, const TcpSocketOptions& opts)
{
    _opts = opts;

    _fd = server.impl().accept();
    PT_LOG_DEBUG("accepted " << _fd);

    init();
}


void TcpSocketImpl::connect(const Endpoint& ep, const TcpSocketOptions& opts)
{
    PT_LOG_DEBUG("connect");

    _opts = opts;

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
            PT_LOG_DEBUG("no more address informations");
            throw System::AccessFailed( _addrInfo.host() );
        }
        
        _fd = WSASocket(_addrInfoPtr->ai_family, SOCK_STREAM, 0, NULL, 0, 0);
        if(_fd == INVALID_SOCKET)
        {
            PT_LOG_DEBUG("create socket failed for: " << _addrInfoPtr->ai_family);
            continue;
        }

        init();

        // set socket to blocking mode
        u_long argp = 0;
        ::ioctlsocket(_fd, FIONBIO, &argp);
        PT_LOG_DEBUG("created socket " << _fd);
        
        socklen_t addrlen = static_cast<socklen_t>(_addrInfoPtr->ai_addrlen);

        if( ::connect(_fd, _addrInfoPtr->ai_addr, addrlen) == 0 )
        {       
            // set socket to non-blocking mode
            argp = 1;
            ::ioctlsocket(_fd, FIONBIO, &argp);
            break;
        }

        close();
    }
}


bool TcpSocketImpl::beginConnect(System::EventLoop& loop, 
                                 const Endpoint& ep, const TcpSocketOptions& opts)
{
    PT_LOG_DEBUG("begin connect");

    _opts = opts;
    _errorPending = false;

    if(_ioh.handle() == INVALID_HANDLE_VALUE)
    {
        loop.selector().enableOverlapped(_ioh);
        PT_LOG_DEBUG("enabled i/o handle " << _ioh.handle());
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
            PT_LOG_DEBUG("connect failed to all possible addresses");
            throw System::AccessFailed( _addrInfo.host() );
        }

        _fd = WSASocket(_addrInfoPtr->ai_family, SOCK_STREAM, 0, NULL, 0, 0);
        if(_fd == INVALID_SOCKET)
        {
            PT_LOG_DEBUG("create socket failed for: " << _addrInfoPtr->ai_family);
            continue;
        }
    
        init();

        // set socket to non-blocking mode
        u_long argp = 1;
        ::ioctlsocket(_fd, FIONBIO, &argp);
        PT_LOG_DEBUG("created socket " << _fd);

        socklen_t addrlen = static_cast<socklen_t>(_addrInfoPtr->ai_addrlen);

        if( ::connect(_fd, _addrInfoPtr->ai_addr, addrlen) == 0 )
        {
            PT_LOG_DEBUG("immediate connect");
            return true;
        }
    
        DWORD lastError = WSAGetLastError();
        if( lastError == WSAEWOULDBLOCK || lastError == WSAEINPROGRESS )
        {
            _eventFlags |= FD_CONNECT;
            eventSelect(_ioh.handle(), _eventFlags);
            PT_LOG_DEBUG("connect in progress");
            return false;
        }
    
        PT_LOG_DEBUG("connect failed, try next address");
        close();
    }
}


void TcpSocketImpl::endConnect(System::EventLoop& loop)
{
    PT_LOG_DEBUG("endConnect on " << _fd);

    // fd is invalid if no address was left to try
    if(_fd != INVALID_SOCKET) 
    {
        _eventFlags &= ~FD_CONNECT;
        eventSelect(_ioh.handle(), _eventFlags);
    }
    
    if(_errorPending)
        throw System::AccessFailed(_addrInfo.host() );

    PT_LOG_INFO("async connect not yet ready socket=" << _fd);

    _eventFlags |= FD_CONNECT;
    eventSelect(_ioh.handle(), _eventFlags);

    bool avail = this->wait(_timeout);

    _eventFlags &= ~FD_CONNECT;
    eventSelect(_ioh.handle(), _eventFlags);

    if(avail)
    {
        WSANETWORKEVENTS events;
        if( WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR )
            throw System::SystemError("WSAEnumNetworkEvents failed");

        if( (events.lNetworkEvents & FD_CLOSE) == FD_CLOSE )
        {
            PT_LOG_DEBUG("received close event " << _fd);
        }
        else if( (events.lNetworkEvents & FD_CONNECT) == FD_CONNECT )
        {
            int s = FD_CONNECT_BIT;
            if(events.iErrorCode[s] == 0)
            {
                PT_LOG_DEBUG("connected " << _fd);
                return;
            }
        }
        else
        {
            PT_LOG_DEBUG("received unknown network event " << _fd);
        }
    }
    
    PT_LOG_DEBUG("failed to connect, try next address " << _fd);
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
        PT_LOG_DEBUG("received FD_CLOSE for connect");
       _errorPending = true;
       return true;
    }

    if( (events.lNetworkEvents & FD_CONNECT) != FD_CONNECT )
    {
        PT_LOG_DEBUG("network events did not contain FD_CONNECT for connect");
        return false;
    }

    int s = FD_CONNECT_BIT;
    if(events.iErrorCode[s] == 0)
    {
        PT_LOG_DEBUG("connect was successful");

        _eventFlags &= ~FD_CONNECT;
        eventSelect(_ioh.handle(), _eventFlags);

        isConnected = true;
        return true;
    }

    PT_LOG_DEBUG("closing socket to try next address");

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


bool TcpSocketImpl::wait(std::size_t msecs)
{
    PT_LOG_DEBUG(_fd << " wait " << msecs);

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


std::size_t TcpSocketImpl::beginRead(System::EventLoop& loop, 
                                     char* buffer, std::size_t n, bool& eof)
{
    PT_LOG_DEBUG(_fd << " beginRead");

    if(_fdClose)
    {
        // FD_CLOSE can be posted at the same time as FD_READ, in which case
        // all remaining data needs to be read from the socket
        int len = ::recv(_fd, buffer, n, 0);
        if(len > 0)
            return len;

        PT_LOG_DEBUG("EOF because of previous FD_CLOSE");
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

    eventSelect(_ioh.handle(), _eventFlags);
    return 0;
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


std::size_t TcpSocketImpl::endRead(System::EventLoop& loop, 
                                   char* buffer, std::size_t, bool& eof)
{
    PT_LOG_DEBUG(_fd << " endRead");
    _eventFlags &= ~FD_READ;

    int len = ::recv(_fd, _receiveBuffer.buf, _receiveBuffer.len, 0);

    if(len == 0)
    {
        eof = true;
    }
    else if(len == -1)
    {
        int err = WSAGetLastError();
        if(err == WSAEWOULDBLOCK)
        {
            // set socket to blocking mode
            eventSelect(0, 0);
        
            u_long argp = 0;
            ::ioctlsocket(_fd, FIONBIO, &argp);
        
            len = ::recv(_fd, _receiveBuffer.buf, _receiveBuffer.len, 0);
        
            if( len == 0)
                eof = true;
        
            // set socket to non-blocking mode
            argp = 1;
            ::ioctlsocket(_fd, FIONBIO, &argp);
        }
        else if(err == WSAECONNRESET)
        {
            eof = true;
        }
    }

    eventSelect(_ioh.handle(), _eventFlags);

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


std::size_t TcpSocketImpl::beginWrite(System::EventLoop& loop, 
                                      const char* buffer, std::size_t n)
{
    PT_LOG_DEBUG(_fd << " beginWrite");

    if(_ioh.handle() == INVALID_HANDLE_VALUE)
    {
        loop.selector().enableOverlapped(_ioh);
    }

    _socketError = 0;
    _bytesSent = 0;

    ULONG maxLen = std::numeric_limits<ULONG>::max();
    _sendBuffer.buf = const_cast<char*>(buffer);
    _sendBuffer.len = n > maxLen ? maxLen : static_cast<ULONG>(n);

    _eventFlags |= FD_WRITE;
    eventSelect(_ioh.handle(), _eventFlags);

    int len = send(_fd, _sendBuffer.buf, _sendBuffer.len, 0);
    if(len < 0)
    {
      if(WSAGetLastError() == WSAEWOULDBLOCK)
      {
        PT_LOG_DEBUG("WSAEWOULDBLOCK on " << _fd);
        return 0;
      }
      else
      {
        PT_LOG_DEBUG("socket error on " << _fd);
        throw System::IOError("send");
      }
    }

    _eventFlags &= ~FD_WRITE;
    eventSelect(_ioh.handle(), _eventFlags);

    return static_cast<std::size_t>(len);
}



bool TcpSocketImpl::runWrite(System::EventLoop& loop)
{
  WSANETWORKEVENTS events;
  int r =  WSAEnumNetworkEvents(_fd, NULL, &events);
  if(r == SOCKET_ERROR)
    throw System::SystemError("WSAEnumNetworkEvents failed");

  if( (events.lNetworkEvents & FD_CLOSE) == FD_CLOSE )
  {
    PT_LOG_DEBUG("FD_CLOSE on write:" << _fdClose);
    _fdClose = true;
    return true;
  }

  if( (events.lNetworkEvents & FD_WRITE) != FD_WRITE )
    return false;

  int ec = events.iErrorCode[FD_WRITE_BIT];
  if(ec != 0)
  {
    _socketError = ec;
    return true;
  }

  int len = send(_fd, _sendBuffer.buf, _sendBuffer.len, 0);
  if(len < 0)
  {
    ec = WSAGetLastError();
    if(ec == WSAEWOULDBLOCK)
    {
      OutputDebugString( "[TcpSocket] WSAEWOULDBLOCK\n");
      return false;
    }

    _socketError = ec;
    return true;
  }

  _bytesSent = static_cast<std::size_t>(len);
  return true;
}


std::size_t TcpSocketImpl::endWrite(System::EventLoop& loop, 
                                    const char* buffer, std::size_t n)
{
    PT_LOG_DEBUG(_fd << " endWrite");

    _eventFlags &= ~FD_WRITE;
    eventSelect(_ioh.handle(), _eventFlags);

    if(_socketError != 0)
      throw System::IOError("send failed");

    return _bytesSent;
}


std::size_t TcpSocketImpl::write(const char* buffer, std::size_t n)
{
    PT_LOG_DEBUG(_fd << " write");

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
    {
      int ec = WSAGetLastError();
      throw System::IOError("WSASend");
    }

    return numberOfBytesSent;
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

