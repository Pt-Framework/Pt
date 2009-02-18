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
#include "TcpSocketImpl.h"
#include "TcpServerImpl.h"
#include "Pt/Net/TcpServer.h"
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/IOError.h"
#include <cerrno>
#include <cstring>
#include <cassert>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//#include <iostream>
#define log_debug(x) //std::cout << x << std::endl;

namespace {

    void formatIp(const sockaddr_storage& addr, std::string& str)
    {
#ifdef HAVE_INET_NTOP
        const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
        char strbuf[INET6_ADDRSTRLEN + 1];
        const char* p = inet_ntop(sa->sin_family, &sa->sin_addr, strbuf, sizeof(strbuf));
        str = (p == 0 ? "-" : strbuf);
#else
        static Pt::System::Mutex monitor;
        Pt::System::MutexLock lock(monitor);

        const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
        const char* p = inet_ntoa(sa->sin_addr);
        if (p)
            str = p;
        else
            str.clear();
#endif
    }
}

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: System::IODeviceImpl(socket)
, _socket(socket)
, _isConnected(false)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
}


void TcpSocketImpl::close()
{
    log_debug("close socket " << _fd);
    System::IODeviceImpl::close();
    _isConnected = false;
}


std::string TcpSocketImpl::getSockAddr() const
{
    struct sockaddr_storage addr;

    socklen_t slen = sizeof(addr);
    if (::getsockname(fd(), reinterpret_cast<struct sockaddr*>(&addr), &slen) < 0)
        throw System::SystemError("getsockname");

    std::string ret;
    formatIp(addr, ret);
    return ret;
}


std::string TcpSocketImpl::getPeerAddr() const
{
    std::string ret;
    formatIp(_peeraddr, ret);
    return ret;
}


void TcpSocketImpl::connect(const std::string& ipaddr, unsigned short int port)
{
    log_debug("connect to " << ipaddr << " port " << port);
    this->beginConnect(ipaddr, port);
    this->endConnect();
}


bool TcpSocketImpl::beginConnect(const std::string& ipaddr, unsigned short int port)
{
    log_debug("begin connect to " << ipaddr << " port " << port);

    if( this->fd() > FD_SETSIZE )
    {
        throw System::IOError( PT_ERROR_MSG("FD_SETSIZE too small for fd") );
    }

    AddrInfo ai(ipaddr, port);

    log_debug("checking address information");
    for (AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        int fd = ::socket(it->ai_family, SOCK_STREAM, 0);
        if (fd < 0)
            continue;

        IODeviceImpl::open(fd, true);

        std::memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);

        log_debug("created socket " << _fd << " max: " << FD_SETSIZE);

        if( ::connect(this->fd(), it->ai_addr, it->ai_addrlen) == 0 )
        {
            _isConnected = true;
            log_debug("connected successfuly");
            return true;
        }

        if(errno != EINPROGRESS)
        {
            close();
            throw System::SystemError("connect failed");
        }

        log_debug("connect in progress");
        if(_wfds)
        {
            FD_SET( this->fd(), _wfds );
        }

        memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);
        return false;
    }

    throw System::SystemError("invalid address information");
}


void TcpSocketImpl::endConnect()
{
    log_debug("ending connect");

    if(_wfds && ! _socket.wbuf() )
    {
        FD_CLR( this->fd(), _wfds );
    }

    if( ! _isConnected )
    {
        try
        {
            fd_set wfds;
            FD_ZERO(&wfds);
            FD_SET(this->fd(), &wfds);
            bool ret = this->wait(_timeout, 0, &wfds, 0);
            if(false == ret)
            {
                throw System::IOTimeout();
            }

            int sockerr;
            socklen_t optlen = sizeof(sockerr);
            if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
            {
                throw System::SystemError("getsockopt");
            }

            if(sockerr != 0)
            {
                throw System::SystemError("connect");
            }

            _isConnected = true;
        }
        catch(...)
        {
            close();
            throw;
        }
    }
}


void TcpSocketImpl::accept(TcpServer& server)
{
    socklen_t peeraddr_len = sizeof(_peeraddr);

    log_debug( "accept " << server.impl().fd() );
    _fd = ::accept(server.impl().fd(), reinterpret_cast <struct sockaddr*>(&_peeraddr), &peeraddr_len);
    if( _fd < 0 )
      throw System::SystemError("accept");

    System::IODeviceImpl::open(_fd, true);
    //TODO ECONNABORTED EINTR EPERM

    _isConnected = true;
    log_debug( "accepted " << server.impl().fd() << " => " << _fd );
}


void TcpSocketImpl::initWait(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    log_debug("TcpSocketImpl::initWait");

    if( this->fd() > 0 )
    {
        if( ! _isConnected )
        {
            FD_SET(this->fd(), &wfds);
        }
    }

    System::IODeviceImpl::initWait(rfds, wfds, efds);
}


int TcpSocketImpl::initSelect(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    log_debug("TcpSocketImpl::initSelect");

    if( this->fd() > 0 )
    {
        if( ! _isConnected )
            FD_SET(this->fd(), &wfds);
    }

    return System::IODeviceImpl::initSelect(rfds, wfds, efds);
}


int TcpSocketImpl::checkEvent(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    log_debug("TcpSocketImpl::checkEvent");

    int avail = 0;

    if( this->fd() < 0)
        return 0;

    if( ! _isConnected && FD_ISSET(this->fd(), &wfds) )
    {
        _socket.connected.send(_socket);
        ++avail;
    }

    avail += System::IODeviceImpl::checkEvent(rfds, wfds, efds);
    return avail;
}

} // namespace Net

} // namespace Pt
