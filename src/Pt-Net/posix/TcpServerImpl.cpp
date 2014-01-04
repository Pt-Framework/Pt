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
#include "TcpServerImpl.h"
#include "MainLoopImpl.h"
#include "IODeviceImpl.h"
#include <Pt/Net/Endpoint.h>
#include <Pt/Net/AddressInUse.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Logger.h>
#include <cerrno>
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/tcp.h>

log_define("Pt.Net.TcpServer")

namespace Pt {

namespace Net {

TcpServerImpl::TcpServerImpl(TcpServer& server)
: _server(server)
, _ioh(server)
, _timeout(Pt::System::EventLoop::WaitInfinite)
, _acceptedFd(-1)
{
}


void TcpServerImpl::create(int domain, int type, int protocol)
{
    log_debug("create socket");
    
    if(_ioh.fd != -1)
    {
        log_debug("closing socket " << _ioh.fd);
        ::close(_ioh.fd);
    }
    
    _ioh.fd = ::socket(domain, type, protocol);
    if (_ioh.fd < 0)
        throw System::IOError("socket");
}


void TcpServerImpl::close()
{
    if(_acceptedFd != -1)
    {
        ::close(_acceptedFd);
        _acceptedFd = -1;
    }

    if (_ioh.fd < 0)
      return;

    log_debug("close socket " << _ioh.fd);

    ::close(_ioh.fd);
    _ioh.fd = -1;
}


void TcpServerImpl::listen(const Endpoint& ep, const TcpServerOptions& options)
{
    static const int on = 1;

    AddrInfo ai;
    ai.resolve(ep, true);

    // getaddrinfo() may return more than one addrinfo structure, so work
    // them all out, until we find a pretty useable one
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
                        ": " << std::strerror(errno));
              close();
              throw System::SystemError("setsockopt IPV6_V6ONLY");
          }
        }
#endif

        log_debug("bind " << _ioh.fd);
        if (::bind(this->fd(), it->ai_addr, it->ai_addrlen) == 0)
        {
            int flags = ::fcntl(_ioh.fd , F_GETFL);
            flags |= O_NONBLOCK;
            if( -1 == ::fcntl(_ioh.fd, F_SETFL, flags) )
            {
                close();
                throw System::SystemError("Could not set O_NONBLOCK");
            }

            // do not allow any forked process to get this fd
            flags = ::fcntl(_ioh.fd, F_GETFD);
            flags |= FD_CLOEXEC;
            if( -1 == ::fcntl(_ioh.fd, F_SETFD, flags) )
            {
                close();
                throw System::SystemError("Could not set FD_CLOEXEC");
            }

            // save our information
            std::memmove(&_servaddr, it->ai_addr, it->ai_addrlen);

            log_debug("listen " << this->fd());
            if( ::listen(this->fd(), options.backlog()) < 0 )
            {
                close();

                if (errno == EADDRINUSE)
                    throw AddressInUse( ep.toString() );
                else
                    throw System::IOError("listen");
            }

#ifdef TCP_DEFER_ACCEPT
            if( options.acceptDeferred() > 0 )
            {
                int deferSecs = options.acceptDeferred();

                log_debug("set TCP_DEFER_ACCEPT to " << deferSecs);

                if( ::setsockopt(this->fd(), SOL_TCP, TCP_DEFER_ACCEPT, &deferSecs, sizeof(deferSecs)) < 0)
                {
                    close();
                    throw System::SystemError("setsockopt TCP_DEFER_ACCEPT");
                }
            }
#endif

            log_debug("successfully listening " << this->fd());
            return;
        }
    }

    close();

    if (errno == EADDRINUSE)
        throw AddressInUse( ep.toString() );
    else
        throw System::IOError("bind");
}
                    
void TcpServerImpl::listen(const std::string& ipaddr,  unsigned short int port,
                           const TcpServerOptions& options)
{

    log_debug("listen on " << ipaddr << " port " << port);

    Endpoint ep(ipaddr, port);
    listen(ep, options); 
}


void TcpServerImpl::beginAccept(System::EventLoop& loop)
{
    if( this->fd() < 0 )
        return;

    log_debug("begin accept " << this->fd());

    sockaddr_storage peeraddr;
    socklen_t peeraddr_len = sizeof(peeraddr);
    int fd = ::accept(this->fd(), reinterpret_cast <struct sockaddr*>(&peeraddr), &peeraddr_len);
    log_debug("accepted: " << fd);

    if(fd != -1)
    {
        _acceptedFd = fd;
        log_debug("immediate accept " << this->fd());
        loop.setReady(_server);
        return;
    }

    if(errno != EAGAIN && errno != EINPROGRESS) // EWOULDBLOCK
    {
        log_debug("accept failed " << this->fd());
        throw System::IOError("accept");
    }

    log_debug("wait for accept " << this->fd());
    loop.selector().beginRead( &_ioh );
}


int TcpServerImpl::accept(const TcpSocketOptions& o)
{
    log_debug( "accept " << this->fd() );

    // connection immediately accepted in beginAccept
    if(_acceptedFd != -1)
    {
        int fd = _acceptedFd;
        _acceptedFd = -1;
        return fd;
    }

    // in case beginAccept was called previously
    if( _server.loop() )
    {
         log_debug("end accept " << this->fd());
        _server.loop()->selector().endRead( &_ioh );
    }

    // in any case block until connection is accepted
    
    //TODO ECONNABORTED EINTR EPERM    
    sockaddr_storage peeraddr;
    socklen_t peeraddr_len = sizeof(peeraddr);
    int fd = -1;

    while(fd < 0)
    {
        fd = ::accept(this->fd(), reinterpret_cast <struct sockaddr*>(&peeraddr), &peeraddr_len);
        if(fd >= 0)
            break;

        if(errno != EAGAIN && errno != EINPROGRESS)
        {
            log_debug("accept failed " << this->fd());
            throw System::IOError("accept");
        }
        
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(this->fd(), &rfds);
        bool avail = System::IODeviceImpl::wait(this->timeout(), &rfds, 0, 0);
        if( ! avail)
            throw System::IOError("accept timeout");
    }

    return fd;
}


void TcpServerImpl::cancel(System::EventLoop& loop)
{
    if(_acceptedFd != -1)
    {
        ::close(_acceptedFd);
        _acceptedFd = -1;
    }

    if( this->fd() < 0 )
        return;

    loop.selector().cancel(_ioh);
}


bool TcpServerImpl::run(System::EventLoop& loop)
{
    if(this->fd() < 0)
        return false;

    if(_acceptedFd != -1)
    {
        return true;
    }

    System::Selector& selector = loop.selector();
    return selector.isReadable(&_ioh);
}

} // namespace Net

} // namespace Pt
