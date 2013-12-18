/*
 * Copyright (C) 2009 Marc Boris Duerner, Tommi Maekitalo
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

#include "TcpServerImpl.h"
#include "AddrInfo.h"
#include <Pt/Net/Endpoint.h>
#include <Pt/Net/AddressInUse.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/System/Logger.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/SystemError.h>

#include <cassert>
#include <cstring>
#include <limits>
//#include <Mswsock.h>

log_define("Pt.System.TcpServer");

namespace Pt {

namespace Net {

static struct WsaInit
{
    WsaInit()
    {
        WSADATA wd; WSAStartup(MAKEWORD(2,2), &wd);
    }
    
    ~WsaInit()
    {
        WSACleanup();
    }
} wsaInit;


TcpServerImpl::TcpServerImpl(TcpServer& server)
: _ioh(server)
, _fd(INVALID_SOCKET)
, _timeout(Pt::System::EventLoop::WaitInfinite)
{
}


TcpServerImpl::~TcpServerImpl()
{
}


void TcpServerImpl::create(int domain, int type, int protocol)
{
    log_debug("create socket");

    _fd = WSASocket(domain, type, protocol, NULL , 0, 0);

    if (_fd == INVALID_SOCKET)
    {
        log_debug("socket() failed: "<< WSAGetLastError());
        throw System::SystemError( PT_ERROR_MSG("creating socket failed") );
    }

    log_debug("server socket " << _fd);
}


void TcpServerImpl::close()
{
    if (_fd == INVALID_SOCKET)
        return;

    log_debug("close socket " << _fd);

    setEventFlags(0, 0);

    ::closesocket(_fd);
    _fd = INVALID_SOCKET;
}


void TcpServerImpl::cancel(System::EventLoop& loop)
{
    // not yet listening
    if (_fd == INVALID_SOCKET || _ioh.handle() == INVALID_HANDLE_VALUE)
        return;

    log_debug("cancel socket " << _fd);
    setEventFlags(_ioh.handle(), 0);
    loop.selector().disableOverlapped(_ioh);
}


void TcpServerImpl::listen(const std::string& ipaddr, unsigned short int port,
                           const TcpServerOptions& options)
{
    Endpoint e(ipaddr, port);
    listen(e, options);
}


void TcpServerImpl::listen(const Endpoint& ep, const TcpServerOptions& options)
{
    log_debug("listen on " << ep.toString());

    BOOL reuseAddr = TRUE;
    static const int on = 1;

    AddrInfo ai;
    ai.resolve(ep, true);

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

        // TODO: use WSA functions

        log_debug("setsockopt SO_REUSEADDR");
        if (::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddr, sizeof(reuseAddr)) < 0)
        {
            close();
            throw System::SystemError("setsockopt");
        }
    
#if defined(IPV6_V6ONLY)
        if (it->ai_family == AF_INET6)
        {
          if (::setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, (const char*) &on, sizeof(on)) < 0)
          {
              log_debug("could not set socket option IPV6_V6ONLY, errno=" << errno << ": " << strerror(errno));
              close();
              throw System::SystemError("setsockopt IPV6_V6ONLY");
          }
        }
#endif
    
        log_debug("bind ");
        socklen_t addrlen = static_cast<socklen_t>(it->ai_addrlen);
        
        if( ::bind(_fd, it->ai_addr, addrlen) == 0 )
        {
   
            log_debug("listen ");
    
            if (::listen(_fd, options.backlog()) == SOCKET_ERROR)
            {
                close();
    
                if (WSAGetLastError() == WSAEADDRINUSE)
                    throw AddressInUse( ep.toString() );
                else
                    throw System::IOError("listen");
            }
    
            return;
        }
    }

    log_debug( "error: " << WSAGetLastError() );
    
    close();

    if (WSAGetLastError() == WSAEADDRINUSE)
        throw AddressInUse( ep.toString() );
    else
        throw System::IOError("bind");
}


void TcpServerImpl::beginAccept(System::EventLoop& loop)
{
    log_debug("begin accepting " << _fd);

    if(_ioh.handle() == INVALID_HANDLE_VALUE)
    {
        loop.selector().enableOverlapped(_ioh);
        log_debug("enabled i/o handle " << _ioh.handle());
    }

    setEventFlags(_ioh.handle(), FD_ACCEPT);
}


SOCKET TcpServerImpl::accept()
{
    setEventFlags(_ioh.handle(), 0);

    SOCKET fd = ::WSAAccept(_fd, NULL, NULL, NULL, 0);
    if(fd == INVALID_SOCKET)
    {
        DWORD err = WSAGetLastError();
        if(WSAEWOULDBLOCK != err) // WSAEINPROGRESS only for blocking WSA 1.1
        {
            log_debug("socket error on " << _fd);
            throw System::IOError("WSAAccept");
        }
        
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(_fd, &fds);
    
        this->waitSelect(&fds, 0, 0, _timeout);

        fd = ::WSAAccept(_fd, NULL, NULL, NULL, 0);
        if(fd == INVALID_SOCKET)
        {
            log_debug("socket error on " << _fd);
            throw System::IOError("WSAAccept");
        }
    }

    log_debug(fd << " accepted ");
    return fd;
}


bool TcpServerImpl::run(System::EventLoop& loop)
{
    log_debug("TcpServerImpl::avail");
    
    if (_fd == INVALID_SOCKET)
        return false;
    
    WSANETWORKEVENTS events;
    if(WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR)
        throw System::SystemError("WSAEnumNetworkEvents failed");

    if((events.lNetworkEvents & FD_ACCEPT) != FD_ACCEPT)
        return false;

    return true;
}


void TcpServerImpl::setEventFlags(HANDLE ev, long events)
{
    if (WSAEventSelect(_fd, ev, events) == SOCKET_ERROR)
    {
        log_debug("Set event failed: "<< WSAGetLastError());
        throw System::SystemError( PT_ERROR_MSG("attach event to server socket failed") );
    }
}


int TcpServerImpl::waitSelect(fd_set* rfds, fd_set* wfds, fd_set* efds, std::size_t timeout)
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
