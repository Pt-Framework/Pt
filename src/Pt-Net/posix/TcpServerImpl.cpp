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
#include "MainLoopImpl.h"
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
, _ioh(server)
{
}


void TcpServerImpl::create(int domain, int type, int protocol)
{
    log_debug("create socket");
    
    _ioh.fd = ::socket(domain, type, protocol);
    if (_ioh.fd < 0)
        throw System::SystemError("socket");
}


void TcpServerImpl::close()
{
    if (_ioh.fd < 0)
      return;

    log_debug("close socket");

    ::close(_ioh.fd);
    _ioh.fd = -1;
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
        if (::setsockopt(this->fd(), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        {
            close();
            throw System::SystemError("setsockopt SO_REUSEADDR");
        }

#ifdef IPPROTO_IPV6
        if (it->ai_family == AF_INET6)
        {
          if (::setsockopt(this->fd(), IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) < 0)
          {
              log_debug("could not set socket option IPV6_V6ONLY, errno=" << errno <<
                        ": " << strerror(errno));
              close();
              throw System::SystemError("setsockopt IPV6_V6ONLY");
          }
        }
#endif

        log_debug("bind");
        if (::bind(this->fd(), it->ai_addr, it->ai_addrlen) == 0)
        {
            // save our information
            std::memmove(&_servaddr, it->ai_addr, it->ai_addrlen);

            log_debug("listen");
            if( ::listen(this->fd(), backlog) < 0 )
            {
                close();

                if (errno == EADDRINUSE)
                    throw AddressInUse();
                else
                    throw System::SystemError("listen");
            }

            int flags = ::fcntl(this->fd(), F_GETFD);
            flags |= FD_CLOEXEC ;
            int ret = ::fcntl(this->fd(), F_SETFD, flags);
            if (ret == -1)
            {
                close();
                throw System::SystemError("Could not set FD_CLOEXEC");
            }

#ifdef TCP_DEFER_ACCEPT
            if( (flags & TcpServer::DeferAccept) != 0 )
            {
                int deferSecs = 30;

                log_debug("set TCP_DEFER_ACCEPT to " << deferSecs);

                if( ::setsockopt(this->fd(), SOL_TCP, TCP_DEFER_ACCEPT, &deferSecs, sizeof(deferSecs)) < 0)
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


void TcpServerImpl::beginAccept(System::EventLoop& loop)
{
    if( this->fd() < 0 )
        return;

    loop.selector().beginRead( &_ioh );
}


void TcpServerImpl::cancel(System::EventLoop& loop)
{
    if( this->fd() < 0 )
        return;

    loop.selector().cancel(_ioh);
}


bool TcpServerImpl::run()
{
    if(this->fd() < 0)
        return false;

    System::Selector& selector = _server.parent()->selector();

    if( selector.isReadable(&_ioh) )
    {
        _server.connectionPending().send(_server);
        return true;
    }

    return false;
}

} // namespace Net

} // namespace Pt
