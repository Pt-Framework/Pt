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
#include <cerrno>
#include <fcntl.h>

//#include <iostream>
#define log_debug(x) //std::cout << x << std::endl;

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: _socket(socket)
, _fd(-1)
, _rfds(0)
, _wfds(0)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
}


void TcpSocketImpl::create(int domain, int type, int protocol)
{
    _fd = ::socket(domain, type, protocol);
    if (_fd < 0)
      throw System::SystemError("socket");
      
    log_debug("create socket " << _fd << " max: " << FD_SETSIZE);
}


void TcpSocketImpl::close()
{
  if (_fd != -1)
  {
    log_debug("close socket " << _fd);
    ::close(_fd);
    _fd = -1;
  }
}


void TcpSocketImpl::connect(const std::string& ipaddr, unsigned short int port)
{
    bool isConnected = this->beginConnect(ipaddr, port);
    if( ! isConnected )
    {
        this->wait(5000); //TODO use timeout value
        this->endConnect();
    }
}


bool TcpSocketImpl::beginConnect(const std::string& ipaddr, unsigned short int port)
{
    log_debug("begin connect to " << ipaddr << " port " << port);

    AddrInfo ai(ipaddr, port);

    log_debug("checking address information");
    for (AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        try
        {
            this->create(it->ai_family, SOCK_STREAM, 0);
        }
        catch (const System::SystemError&)
        {
            continue;
        }

        int flags = fcntl(_fd, F_GETFL);
        flags |= O_NONBLOCK ;
        fcntl(_fd, F_SETFL, O_NONBLOCK);

        memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);

        log_debug("created socket, tying connect");

        if( ::connect(this->fd(), it->ai_addr, it->ai_addrlen) == 0 )
        {
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
    
    if(_wfds)
	{
		FD_CLR( this->fd(), _wfds );
	}
    
    int sockerr;
    socklen_t optlen = sizeof(sockerr);
    if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
    {
        this->close();
        throw System::SystemError("getsockopt");
    }

    if(sockerr != 0)
    {
        this->close();
        throw System::SystemError("connect");
    }
}


void TcpSocketImpl::accept(TcpServer& server)
{
    socklen_t peeraddr_len = sizeof(_peeraddr);

    log_debug( "accept " << server.impl().fd() );
    _fd = ::accept(server.impl().fd(), reinterpret_cast <struct sockaddr*>(&_peeraddr), &peeraddr_len);
    if( _fd < 0 )
      throw System::SystemError("accept");

    //TODO ECONNABORTED EINTR EPERM

    log_debug( "accepted " << server.impl().fd() << " => " << _fd );
}


bool TcpSocketImpl::wait(std::size_t msecs)
{
    log_debug("wait " << msecs);

    if( this->fd() > FD_SETSIZE )
    {
        throw System::IOError( PT_ERROR_MSG("FD_SETSIZE too small for fd") );
    }

    struct timeval* timeout = 0;
    struct timeval tv;
    if(msecs != System::Selector::WaitInfinite)
    {
        tv.tv_sec = msecs / 1000;
        tv.tv_usec = (msecs % 1000) * 1000;
        timeout = &tv;
    }

    fd_set rfds;
    fd_set wfds;
    fd_set efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    if( this->fd() > 0 )
    {
        FD_SET(this->fd(), &wfds);
    }

    while( true )
    {
        int ret = ::select(this->fd() + 1, 0, &wfds, 0, timeout);
        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw System::IOError( "select failed" );
    }

    return checkEvent(rfds, wfds, efds);
}


void TcpSocketImpl::attach(System::SelectorBase& sb)
{
    log_debug("attach to selector");
    if( this->fd() > FD_SETSIZE )
    {
        throw System::IOError( PT_ERROR_MSG("FD_SETSIZE too small for fd") );
    }
}


void TcpSocketImpl::detach(System::SelectorBase& sb)
{
    log_debug("detach from selector " << _fd);
    this->exitSelect();
}


int TcpSocketImpl::initSelect(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    log_debug("TcpSocketImpl::initSelect");
   
    _rfds = &rfds;
    _wfds = &wfds;

    if( this->fd() > 0)
    {
        FD_SET(this->fd(), _wfds);
    }

    return this->fd();
}


void TcpSocketImpl::exitSelect()
{
    log_debug("TcpSocketImpl::exitSelect " << _fd);
    
    if( _wfds && this->fd() > 0)
    {
        FD_CLR(this->fd(), _wfds);
    }

    if( _rfds && this->fd() > 0)
    {
        FD_CLR(this->fd(), _rfds);
    }
    
    _rfds = 0;
    _wfds = 0;
}


int TcpSocketImpl::checkEvent(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    log_debug("TcpSocketImpl::checkEvent");

    if( this->fd() < 0)
        return 0;

    if( FD_ISSET(this->fd(), &wfds) )
    {
        _socket.connected.send(_socket);
        return 1;
    }

    return 0;
}

} // namespace Net

} // namespace Pt
