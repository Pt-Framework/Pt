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

#include <Pt/Net/AddrInfo.h>
#include "TcpServerImpl.h"
#include "AddrInfoImpl.h"
#include <Pt/Net/TcpServer.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/EventLoop.h>
#include <cerrno>
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/tcp.h>

#define log_debug(x)

namespace Pt {

namespace Net {

TcpServerImpl::TcpServerImpl(TcpServer& server)
: _server(server)
, _fd(-1)
, _rfds(0)
{
}


void TcpServerImpl::create(int domain, int type, int protocol)
{
  log_debug("create socket");

  _fd = ::socket(domain, type, protocol);
  if (_fd < 0)
    throw System::SystemError("socket");
}


void TcpServerImpl::close()
{
  if (_fd < 0)
      return;

    log_debug("close socket");

    ::close(_fd);
    _fd = -1;
}


void TcpServerImpl::listen(const std::string& ipaddr,
                           unsigned short int port,
                           int backlog, unsigned flags)
{
    log_debug("listen on " << ipaddr << " port " << port
              << " backlog " << backlog << " flags " << flags);

    AddrInfo ai(ipaddr, port, true);

    static const int on = 1;

    // getaddrinfo() may return more than one addrinfo structure, so work
    // them all out, until we find a pretty useable one
    for (AddrInfoImpl::const_iterator it = ai.impl()->begin(); it != ai.impl()->end(); ++it)
    {
        try
        {
            this->create(it->ai_family, SOCK_STREAM, 0);
        }
        catch (const System::SystemError&)
        {
            continue;
        }

        log_debug("setsockopt SO_REUSEADDR");
        if (::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        {
            close();
            throw System::SystemError("setsockopt SO_REUSEADDR");
        }

#ifdef IPPROTO_IPV6
        if (it->ai_family == AF_INET6)
        {
          if (::setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) < 0)
          {
              log_debug("could not set socket option IPV6_V6ONLY, errno=" << errno <<
                        ": " << strerror(errno));
              close();
              throw System::SystemError("setsockopt IPV6_V6ONLY");
          }
        }
#endif

        log_debug("bind");
        if (::bind(_fd, it->ai_addr, it->ai_addrlen) == 0)
        {
            // save our information
            std::memmove(&_servaddr, it->ai_addr, it->ai_addrlen);

            log_debug("listen");
            if( ::listen(_fd, backlog) < 0 )
            {
                close();

                if (errno == EADDRINUSE)
                    throw AddressInUse();
                else
                    throw System::SystemError("listen");
            }

            if( (flags & TcpServer::INHERIT) == 0 )
            {
                int flags = ::fcntl(_fd, F_GETFD);
                flags |= FD_CLOEXEC ;
                int ret = ::fcntl(_fd, F_SETFD, flags);
                if (ret == -1)
                {
                    close();
                    throw System::SystemError("Could not set FD_CLOEXEC");
                }
            }

#ifdef TCP_DEFER_ACCEPT
            if( (flags & TcpServer::DEFER_ACCEPT) != 0 )
            {
                int deferSecs = 30;

                log_debug("set TCP_DEFER_ACCEPT to " << deferSecs);

                if( ::setsockopt(_fd, SOL_TCP, TCP_DEFER_ACCEPT, &deferSecs, sizeof(deferSecs)) < 0)
                {
                    close();
                    throw System::SystemError("setsockopt TCP_DEFER_ACCEPT");
                }
            }
#endif
            return;
        }
    }

    close();

    if (errno == EADDRINUSE)
        throw AddressInUse(ipaddr, port);
    else
        throw System::SystemError("bind");
}


bool TcpServerImpl::wait(std::size_t msecs)
{
    log_debug("wait " << msecs);

    if( this->fd() > FD_SETSIZE )
    {
        throw System::IOError( PT_ERROR_MSG("FD_SETSIZE too small for fd") );
    }

    fd_set rfds;
    fd_set efds;
    FD_ZERO(&rfds);
    FD_ZERO(&efds);

    struct timeval* timeout = 0;
    struct timeval tv;
    if(msecs != System::EventLoop::WaitInfinite)
    {
        tv.tv_sec = msecs / 1000;
        tv.tv_usec = (msecs % 1000) * 1000;
        timeout = &tv;
    }

    if( this->fd() > 0 )
    {
        FD_SET(this->fd(), &rfds);
        FD_SET(this->fd(), &efds);
    }

    while( true )
    {
        int ret = ::select(this->fd() + 1, &rfds, 0, &efds, timeout);
        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw System::IOError( "select failed" );
    }

    int avail = 0;

    if( FD_ISSET(this->fd(), &rfds) )
    {
        _server.connectionPending.send(_server);
        ++avail;
    }

    return avail != 0;
}


void TcpServerImpl::attach(System::EventLoop& s)
{
    log_debug("attach to selector");

    if( this->fd() > FD_SETSIZE )
    {
        throw System::IOError( PT_ERROR_MSG("FD_SETSIZE too small for fd") );
    }
}


void TcpServerImpl::detach(System::EventLoop& s)
{
    log_debug("detach from selector");
    this->exitSelect();
}


int TcpServerImpl::initSelect(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    _rfds = &rfds;

    if( this->fd() > 0)
    {
        FD_SET(this->fd(), _rfds);
    }

    return this->fd();
}


void TcpServerImpl::exitSelect()
{
    if( _rfds && this->fd() > 0)
    {
        FD_CLR(this->fd(), _rfds);
    }

    _rfds = 0;
}


int TcpServerImpl::checkEvent(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    if( this->fd() < 0)
        return 0;

    if( FD_ISSET(this->fd(), &rfds) )
    {
        _server.connectionPending.send(_server);
        return 1;
    }

    return 0;
}

} // namespace Net

} // namespace Pt
