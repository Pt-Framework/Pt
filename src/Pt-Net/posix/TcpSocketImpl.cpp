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

#include "Pt/Net/AddrInfo.h"
#include "TcpSocketImpl.h"
#include "TcpServerImpl.h"
#include "MainLoopImpl.h"
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

log_define("Pt.Net.TcpSocket");

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: System::IODeviceImpl(socket)
, _socket(socket)
, _errorPending(false)
, _isConnected(false)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
}


void TcpSocketImpl::close()
{
    if( this->isOpen() )
    {
        log_debug("close socket " << fd());
        System::IODeviceImpl::close();
    }

    _isConnected = false;
    _errorPending = false;
}


void TcpSocketImpl::cancel(System::EventLoop& loop)
{
    if( this->isOpen() )
    {
        log_debug("cancel socket " << fd());
        IODeviceImpl::cancel(loop);
    }

    _errorPending = false;
}


void TcpSocketImpl::accept(const TcpServer& server, const TcpSocket::Options& o)
{
    int fd = server.impl().accept(o);

    bool inherit = false;
    System::IODeviceImpl::open(fd, inherit);

    _isConnected = true;
    log_debug( "accepted " << server.impl().fd() << " => " << this->fd() );
}


void TcpSocketImpl::connect(const Endpoint& ep)
{
    log_trace("connect");
    assert( ! _isConnected );

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
            log_info("could not connect to any address");
            throw System::AccessFailed( _addrInfo.host() );
        }

        int fd = ::socket(_addrInfoPtr->ai_family, SOCK_STREAM, 0);
        if (fd < 0)
        {
            log_debug("failed to create socket for address " << this->fd());
            continue;
        }

        IODeviceImpl::open(fd, false);
        log_info("created socket " << this->fd());

        if( ::connect(this->fd(), _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0 )
        {
            log_debug("connect imediately sucessful " << this->fd());
            _isConnected = true;
            break;
        }

        if (errno != EINPROGRESS)
        {
            log_debug("connect failed " << this->fd());
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
                log_warn("getsockopt failed " << this->fd());
                close();
                throw System::SystemError("getsockopt");
            }
        
            if (sockerr == 0)
            {
                log_debug("connect sucessful");
                _isConnected = true;
                break;
            }
        }
        
        log_debug("connect failed for address" << this->fd());
        close();
    }
}


bool TcpSocketImpl::beginConnect(System::EventLoop& loop, const Endpoint& ep)
{
    log_trace("begin connect");
    assert( ! _isConnected );

    _errorPending = false;
    
    _addrInfo.resolve(ep);
    _addrInfoPtr = _addrInfo.begin();

    /*while(true)
    {
        if(_addrInfoPtr == _addrInfo.impl()->end())
        {
            log_debug("connect failed to all possible addresses");
            throw System::AccessFailed( _addrInfo.host() );
        }

        try
        {
            _isConnected = beginConnect(loop, *_addrInfoPtr);
            break;
        } 
        catch(const System::IOError&)
        { }

        ++_addrInfoPtr;
    }*/

    /// NEW:
    _isConnected = beginConnect(loop);

    return _isConnected;
}


bool TcpSocketImpl::beginConnect(System::EventLoop& loop)
{
    for( ; ; ++_addrInfoPtr)
    {
        if(_addrInfoPtr == _addrInfo.end())
        {
            log_debug("connect failed to all possible addresses");
            throw System::AccessFailed( _addrInfo.host() );
        }

        log_trace("begin connect to addr family:" << _addrInfoPtr->ai_family << ", socktype:" << _addrInfoPtr->ai_socktype);
        assert( ! _isConnected );
    
        int fd = ::socket(_addrInfoPtr->ai_family, SOCK_STREAM, 0);
        if (fd < 0)
        {
            log_debug("failed to create socket for address " << this->fd());
            continue;
        }
    
        IODeviceImpl::open(fd, false);
        log_debug("created socket " << this->fd());
    
        if( ::connect(this->fd(), _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0 )
        {
            log_debug("connect imediately successful " << this->fd());
            _isConnected = true;
            return true;
        }
   
        if (errno == EINPROGRESS)
        { 
            log_debug("connect in progress " << this->fd());
            loop.selector().beginWrite( &_ioh );
            return false;
        }

        log_debug("connect failed for address " << this->fd());
        close();
    }
}


/*bool TcpSocketImpl::beginConnect(System::EventLoop& loop, const ::addrinfo& 
ai) { 
    log_trace("begin connect to addr family:" << ai.ai_family << ", socktype:" << ai.ai_socktype);
    assert( ! _isConnected );

    int fd = ::socket(ai.ai_family, SOCK_STREAM, 0);
    if (fd < 0)
    {
        log_debug("failed to create socket for address " << this->fd());
        throw System::IOError("socket");
    }

    IODeviceImpl::open(fd, false);
    log_debug("created socket " << this->fd());

    if( ::connect(this->fd(), ai.ai_addr, ai.ai_addrlen) == 0 )
    {
        log_debug("connect imediately successful " << this->fd());
        _isConnected = true;
        return true;
    }

    if (errno != EINPROGRESS)
    {
        log_debug("connect failed for address " << this->fd());
        close();
        throw System::IOError("connect");
    }

    log_debug("connect in progress " << this->fd());
    loop.selector().beginWrite( &_ioh );
    return false;
}*/


void TcpSocketImpl::endConnect(System::EventLoop& loop)
{
    // endConnect is only called if a async connect is running, however
    // the user might have called it in response to a ready notification
    // or not.
    log_trace("ending connect");

    if(_errorPending)
    {
        // _errorPending is set in runConnect if an error was detected and
        // connect could not be restarted for te next resolved address. In
        // either case, the i/o handle is not registered anymore.
        log_debug("pending error " << this->fd());
        throw System::AccessFailed( _addrInfo.host() );
    }

    if(_isConnected)
    {
        // _isConnected is set in runConnect if the socket was writable 
        // and no error was detected. The i/o handle no longer waits for 
        // writability.
        log_debug("connected successfully " << this->fd());
        return;
    }

    loop.selector().endWrite( &_ioh );

    log_info("wait for connect to finish");

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
            log_debug("connected successfully");
            _isConnected = true;
            return;
        }
    }

    log_debug("failed to connect, try next address " << this->fd());
    cancel(loop);
    close();

    ++_addrInfoPtr;
    this->connect();
}


bool TcpSocketImpl::runConnect(System::EventLoop& loop)
{
    log_trace("runConnect");

    System::Selector& selector = loop.selector();

    if( selector.isError(&_ioh) )
    {
        log_debug("socket has error");

        /*while(true)
        {
            cancel(loop);
            close();

            if( ++_addrInfoPtr == _addrInfo.impl()->end() )
            {
                log_debug("no usable address, connect failed");
                _errorPending = true;
                return true;
            }

            try 
            {
                log_debug("trying next address");

                // if this returns true the i/o handle is not in the loop
                return beginConnect(loop, *_addrInfoPtr);
            }
            catch(const System::IOError& )
            { }
        }*/

        /// NEW:
        cancel(loop);
        close();

        try
        {
            ++_addrInfoPtr;
            _isConnected = this->beginConnect(loop);
        }
        catch(const System::AccessFailed& )
        { 
            _errorPending = true;
        }

        return _isConnected || _errorPending;
    }
    else if( selector.isWritable(&_ioh)  )
    {
        log_debug("socket is writable");

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
            log_debug("connected successfully");
            loop.selector().endWrite( &_ioh );
            _isConnected = true;
            return true;
        }

        /*while(true)
        {
            cancel(loop);
            close();

            if( ++_addrInfoPtr == _addrInfo.impl()->end() )
            {
                log_debug("no usable address to connect");
                _errorPending = true;
                return true;
            }

            try 
            {
                log_debug("trying next address");

                // if this returns true the i/o handle is not in the loop
                return beginConnect(loop, *_addrInfoPtr);
            }
            catch(const System::IOError& )
            { }
        }*/

        /// NEW:
        cancel(loop);
        close();

        try
        {
            ++_addrInfoPtr;
            _isConnected = this->beginConnect(loop);
        }
        catch(const System::AccessFailed& )
        { 
            _errorPending = true;
        }

        return _isConnected || _errorPending;
    }

    return false;
}


void TcpSocketImpl::localEndpoint(Endpoint& ep) const
{
    struct sockaddr_storage addr;
    socklen_t slen = sizeof(addr);

    int ret = ::getsockname(fd(), reinterpret_cast<struct sockaddr*>(&addr), &slen);

    if(ret == 0)
        ep.impl()->init( &sockadr, slen );
    else
        ep.clear();
}


void TcpSocketImpl::remoteEndpoint(Endpoint& ep) const
{
    struct sockaddr_storage addr;
    socklen_t slen = sizeof(addr);

    int ret = ::getpeername(fd(), reinterpret_cast<struct sockaddr*>(&addr), &slen);

    if(ret == 0)
        ep.impl()->init( &sockadr, slen );
    else
        ep.clear();
}

} // namespace Net

} // namespace Pt

