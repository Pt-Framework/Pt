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
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

log_define("Pt.Net.TcpSocket");

namespace {

void addressToString(const sockaddr_storage& addr, std::string& str)
{
#ifdef PT_WITH_INET_NTOA
    static Pt::System::Mutex monitor;
    Pt::System::MutexLock lock(monitor);

    const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
    const char* p = inet_ntoa(sa->sin_addr);
    if (p)
        str = p;
    else
        str.clear();
#else
    const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
    char strbuf[INET6_ADDRSTRLEN + 1];
    const char* p = inet_ntop(sa->sin_family, &sa->sin_addr, strbuf, sizeof(strbuf));
    str = (p == 0 ? "-" : strbuf);
#endif
}

}

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
    log_debug("close socket " << fd());
    System::IODeviceImpl::close();
    _isConnected = false;
    _errorPending = false;
}


void TcpSocketImpl::cancel(System::EventLoop& loop)
{
    log_debug("cancel socket " << fd());
    _errorPending = false;
    IODeviceImpl::cancel(loop);
}


void TcpSocketImpl::accept(const TcpServer& server, unsigned flags)
{
    bool inherit = false;

    sockaddr_storage peeraddr;
    socklen_t peeraddr_len = sizeof(peeraddr);

    log_debug( "accept " << server.impl().fd() );
    int fd = ::accept(server.impl().fd(), reinterpret_cast <struct sockaddr*>(&peeraddr), &peeraddr_len);
    if( fd < 0 )
      throw System::SystemError("accept");

    System::IODeviceImpl::open(fd, inherit);
    //TODO ECONNABORTED EINTR EPERM

    _isConnected = true;
    log_debug( "accepted " << server.impl().fd() << " => " << this->fd() );
}


void TcpSocketImpl::connect(const AddrInfo& addrInfo)
{
    log_trace("connect");
    assert( ! _isConnected );

    _addrInfo = addrInfo;
    _addrInfoPtr = _addrInfo.impl()->begin();

    for( ; _addrInfoPtr != _addrInfo.impl()->end(); ++_addrInfoPtr)
    {
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
            log_debug("connect imediately successful " << this->fd());
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
        if( ! avail)
            throw System::IOTimeout();

        int sockerr;
        socklen_t optlen = sizeof(sockerr);
    
        // check for socket error
        if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
        {
            log_warn("getsockopt failed " << this->fd());
            close();
            throw System::SystemError("getsockopt");
        }
    
        if (sockerr == 0)
        {
            log_debug("connected successfully");
            _isConnected = true;
            break;
        }
        
        log_debug("connect failed for address" << this->fd());
        close();
    }

    if(_addrInfoPtr == _addrInfo.impl()->end())
    {
        log_info("could not connect to any address");
        throw System::IOError("connect failed");
    }
}


bool TcpSocketImpl::beginConnect(System::EventLoop& loop, const AddrInfo& addrInfo)
{
    log_trace("begin connect");
    assert( ! _isConnected );

    _errorPending = false;
    _addrInfo = addrInfo;
    _addrInfoPtr = _addrInfo.impl()->begin();

    while(true)
    {
        if(_addrInfoPtr == _addrInfo.impl()->end())
        {
            log_debug("connect failed to all possible addresses");
            throw System::IOError("connect failed");
        }

        try
        {
            _isConnected = beginConnect(*_addrInfoPtr);
            break;
        } 
        catch(const System::IOError&)
        { }

        ++_addrInfoPtr;
    }

    return _isConnected;
}


bool TcpSocketImpl::beginConnect(const ::addrinfo& ai)
{
    log_trace("begin connect");
    assert( ! _isConnected );

    int fd = ::socket(ai.ai_family, SOCK_STREAM, 0);
    if (fd < 0)
    {
        log_debug("failed to create socket for address " << this->fd());
        throw IOError("socket")
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
        throw IOError("connect");
    }

    log_debug("connect in progress " << this->fd());
    loop.selector().beginWrite( &_ioh );
    return false;
}


void TcpSocketImpl::endConnect(System::EventLoop& loop)
{
    log_trace("ending connect");

    // we do not need to remove the IOHandle from the selector, this has
    // already been done in runConnect().

    if( _isConnected )
    {
        log_debug("connected successfully " << _fd);
        return;
    }

    if(_errorPending)
    {
        log_debug("pending error " << _fd);
        throw System::IOError("connect");
    }

    log_info("ending async connect without waiting");

    try
    {
        bool hasTimeout = false;
        while (true)
        {
            fd_set wfds;
            FD_ZERO(&wfds);
            FD_SET(this->fd(), &wfds);

            bool avail = this->wait(timeout(), 0, &wfds, 0);
            if( ! avail )
                hasTimeout = true;

            if (avail)
            {
                int sockerr = 0;
                socklen_t optlen = sizeof(sockerr);
                if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
                {
                    close();

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

            log_debug("failed to connect, try next address " << _fd);

            while(true)
            {
                cancel();
                close();
    
                if( ++_addrInfoPtr == _addrInfo.impl()->end() )
                {
                    log_debug("no more addresses to try");
                    if(hasTimeout)
                        throw System::IOTimeout();
                    else
                        throw System::IOError("connect");
                }
    
                try 
                {
                    log_debug("trying next address");
                    return beginConnect(*_addrInfoPtr);
                }
                catch(const IOError& )
                { }
            }
        }
    }
    catch(...)
    {
        cancel();
        close();
        throw;
    }
}


bool TcpSocketImpl::runConnect(System::EventLoop& loop)
{
    log_trace("runConnect");

    System::Selector& selector = loop.selector();

    if( selector.isError(&_ioh) )
    {
        log_debug("socket has error");

        while(true)
        {
            cancel();
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
                return beginConnect(*_addrInfoPtr);
            }
            catch(const IOError& )
            { }
        }
    }
    else if( selector.isWritable(&_ioh)  )
    {
        log_debug("socket is writable");

        int sockerr = 0;
        socklen_t optlen = sizeof(sockerr);
        if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
        {
            cancel();
            close();

            // getsockopt failure is not an I/O error
            throw System::SystemError("getsockopt");
        }
    
        if (sockerr == 0)
        {
            log_debug("ending write " << _fd);
            loop.selector().endWrite( &_ioh );

            log_debug("connected successfully");
            _isConnected = true;
            return true;
        }

        while(true)
        {
            cancel();
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
                return beginConnect(*_addrInfoPtr);
            }
            catch(const IOError& )
            { }
        }
    }

    return false;
}


std::string TcpSocketImpl::socketAddress() const
{
    struct sockaddr_storage addr;

    socklen_t slen = sizeof(addr);
    if (::getsockname(fd(), reinterpret_cast<struct sockaddr*>(&addr), &slen) < 0)
        throw System::SystemError("getsockname");

    std::string ret;
    addressToString(addr, ret);
    return ret;
}


std::string TcpSocketImpl::peerAddress() const
{
    struct sockaddr_storage addr;

    socklen_t slen = sizeof(addr);
    if (::getpeername(fd(), reinterpret_cast<struct sockaddr*>(&addr), &slen) < 0)
        throw System::SystemError("getsockname");

    std::string ret;
    addressToString(addr, ret);
    return ret;
}

} // namespace Net

} // namespace Pt

