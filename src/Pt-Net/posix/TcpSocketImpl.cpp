/*
 * Copyright (C) 2009 Marc Boris Duerner, Tommi Maekitalo
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

#include "AddrInfo.h"
#include "EndpointImpl.h"
#include "TcpSocketImpl.h"
#include "TcpServerImpl.h"
#include "MainLoopImpl.h"
#include "Pt/Net/Endpoint.h"
#include "Pt/Net/TcpServer.h"
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/Logger.h"
#include "Pt/System/IOError.h"
#include <cerrno>
#include <cstring>
#include <cassert>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

PT_LOG_DEFINE("Pt.Net.TcpSocket");

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: System::IODeviceImpl(socket)
, _socket(socket)
, _errorPending(false)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
}


void TcpSocketImpl::init(int fd)
{
    bool inherit = false;
    IODeviceImpl::open(fd, inherit);
    PT_LOG_DEBUG( "created socket " << this->fd() );

    if( _opts.keepAlive() >= 0 )
    {
        PT_LOG_DEBUG("using SO_KEEPALIVE");

        int on = 1;
        if( ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)) ) 
        {
            close();
            throw System::SystemError("setsockopt SO_KEEPALIVE");
        }
    }
        
    if( _opts.keepAlive() > 0 )
    {
        PT_LOG_DEBUG( "using keep-alive: " << _opts.keepAlive() );

#if defined(TCP_KEEPIDLE)
        int secs = _opts.keepAlive();
        if( ::setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, &secs, sizeof(secs)) ) 
        {
            close();
            throw System::SystemError("setsockopt TCP_KEEPIDLE");
        }

        if( ::setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &secs, sizeof(secs)) )
        {
            close();
            throw System::SystemError("setsockopt TCP_KEEPINTVL");
        }
#elif defined(TCP_KEEPALIVE)
        int secs = _opts.keepAlive();
        if( ::setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE, &secs, sizeof(secs)) )
        {
            close();
            throw System::SystemError("setsockopt TCP_KEEPALIVE");
        }
#endif
    }
}


void TcpSocketImpl::close()
{
    if( this->isOpen() )
    {
        PT_LOG_DEBUG("close socket " << fd());
        System::IODeviceImpl::close();
    }

    _errorPending = false;
}


void TcpSocketImpl::cancel(System::EventLoop& loop)
{
    if( this->isOpen() )
    {
        PT_LOG_DEBUG("cancel socket " << fd());
        IODeviceImpl::cancel(loop);
    }

    _errorPending = false;
}


void TcpSocketImpl::accept(TcpServer& server, const TcpSocketOptions& opts)
{
    _opts = opts;

    int fd = server.impl().accept(opts);
    init(fd);

    PT_LOG_DEBUG( "accepted " << server.impl().fd() << " => " << this->fd() );
}


void TcpSocketImpl::connect(const Endpoint& ep, const TcpSocketOptions& opts)
{
    PT_LOG_TRACE("connect");

    _opts = opts;

    _addrInfo.resolve(ep);
    _addrInfoPtr = _addrInfo.begin();

    this->connect();
}


void TcpSocketImpl::connect()
{
    for( ; ; ++_addrInfoPtr)
    {
        if(_addrInfoPtr == _addrInfo.end())
        {
            PT_LOG_INFO("could not connect to any address");
            throw System::AccessFailed( _addrInfo.host() );
        }

        PT_LOG_DEBUG("connect to addr family:" << _addrInfoPtr->ai_family << ", socktype:" << _addrInfoPtr->ai_socktype);

        int fd = ::socket(_addrInfoPtr->ai_family, SOCK_STREAM, 0);
        if(fd < 0)
        {
            PT_LOG_DEBUG( "create socket failed for domain: " << _addrInfoPtr->ai_family );
            continue;
        }

        init(fd);

        if( ::connect(this->fd(), _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0 )
        {
            PT_LOG_DEBUG("connect imediately sucessful " << this->fd());
            break;
        }

        if (errno != EINPROGRESS)
        {
            PT_LOG_DEBUG("connect failed " << this->fd());
            close();
            continue;
        }

        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(this->fd(), &wfds);
        bool avail = this->wait(timeout(), 0, &wfds, 0);
        if(avail)
        {
            int sockerr;
            socklen_t optlen = sizeof(sockerr);
            if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
            {
                PT_LOG_WARN("getsockopt failed " << this->fd());
                close();
                throw System::SystemError("getsockopt");
            }
        
            if (sockerr == 0)
            {
                PT_LOG_DEBUG("connect sucessful");
                break;
            }
        }
        
        PT_LOG_DEBUG("connect failed for address" << this->fd());
        close();
    }
}


bool TcpSocketImpl::beginConnect(System::EventLoop& loop, 
                                 const Endpoint& ep, const TcpSocketOptions& opts)
{
    PT_LOG_TRACE("begin connect");

    _opts = opts;
    _errorPending = false;
    
    _addrInfo.resolve(ep);
    _addrInfoPtr = _addrInfo.begin();

    return beginConnect(loop);
}


bool TcpSocketImpl::beginConnect(System::EventLoop& loop)
{
    for( ; ; ++_addrInfoPtr)
    {
        if(_addrInfoPtr == _addrInfo.end())
        {
            PT_LOG_DEBUG("connect failed to all possible addresses");
            throw System::AccessFailed( _addrInfo.host() );
        }

        PT_LOG_DEBUG("begin connect to addr family:" << _addrInfoPtr->ai_family << ", socktype:" << _addrInfoPtr->ai_socktype);

        int fd = ::socket(_addrInfoPtr->ai_family, SOCK_STREAM, 0);
        if(fd < 0)
        {
            PT_LOG_DEBUG( "create socket failed");
            continue;
        }

        init(fd);

        if( ::connect(this->fd(), _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0 )
        {
            PT_LOG_DEBUG("connect imediately successful " << this->fd());
            return true;
        }
   
        if (errno == EINPROGRESS)
        { 
            PT_LOG_DEBUG("connect in progress " << this->fd());
            loop.selector().beginWrite( &_ioh );
            return false;
        }

        PT_LOG_DEBUG("connect failed for address " << this->fd());
        close();
    }
}


void TcpSocketImpl::endConnect(System::EventLoop& loop)
{
    // endConnect is only called if a async connect is running, however
    // the user might have called it in response to a ready notification
    // or not.
    PT_LOG_TRACE("ending connect");

    if(_errorPending)
    {
        // _errorPending is set in runConnect if an error was detected and
        // connect could not be restarted for te next resolved address. In
        // either case, the i/o handle is not registered anymore.
        PT_LOG_DEBUG("pending error " << this->fd());
        throw System::AccessFailed( _addrInfo.host() );
    }

    loop.selector().endWrite( &_ioh );

    PT_LOG_INFO("wait for connect to finish");

    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(this->fd(), &wfds);
    bool avail = this->wait(timeout(), 0, &wfds, 0);
    if (avail)
    {
        int sockerr = 0;
        socklen_t optlen = sizeof(sockerr);
        if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
        {
            // getsockopt failed, not an I/O error
            throw System::SystemError("getsockopt");
        }
    
        if (sockerr == 0)
        {
            PT_LOG_DEBUG("connected successfully");
            return;
        }
    }

    PT_LOG_DEBUG("failed to connect, try next address " << this->fd());
    cancel(loop);
    close();

    ++_addrInfoPtr;
    this->connect();
}


bool TcpSocketImpl::runConnect(System::EventLoop& loop, bool& isConnected)
{
    PT_LOG_TRACE("runConnect");

    System::Selector& selector = loop.selector();

    if( selector.isError(&_ioh) )
    {
        PT_LOG_DEBUG("socket has error");

        cancel(loop);
        close();

        try
        {
            ++_addrInfoPtr;
            isConnected = this->beginConnect(loop);
        }
        catch(const System::AccessFailed& )
        { 
            _errorPending = true;
        }

        return isConnected || _errorPending;
    }
    else if( selector.isWritable(&_ioh)  )
    {
        PT_LOG_DEBUG("socket is writable");

        int sockerr = 0;
        socklen_t optlen = sizeof(sockerr);
        if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
        {
            cancel(loop);
            close();

            // getsockopt failure is not an I/O error
            throw System::SystemError("getsockopt");
        }
    
        if (sockerr == 0)
        {
            PT_LOG_DEBUG("connected successfully");
            loop.selector().endWrite( &_ioh );
            isConnected = true;
            return true;
        }

        cancel(loop);
        close();

        try
        {
            ++_addrInfoPtr;
            isConnected = this->beginConnect(loop);
        }
        catch(const System::AccessFailed& )
        { 
            _errorPending = true;
        }

        return isConnected || _errorPending;
    }

    return false;
}


void TcpSocketImpl::localEndpoint(Endpoint& ep) const
{
    struct sockaddr_storage addr;
    socklen_t slen = sizeof(addr);

    int ret = ::getsockname(fd(), reinterpret_cast<struct sockaddr*>(&addr), &slen);

    if(ret == 0)
        ep.impl()->init( (sockaddr*)&addr, slen );
    else
        ep.clear();
}


void TcpSocketImpl::remoteEndpoint(Endpoint& ep) const
{
    struct sockaddr_storage addr;
    socklen_t slen = sizeof(addr);

    int ret = ::getpeername(fd(), reinterpret_cast<struct sockaddr*>(&addr), &slen);

    if(ret == 0)
        ep.impl()->init( (sockaddr*)&addr, slen );
    else
        ep.clear();
}

} // namespace Net

} // namespace Pt

