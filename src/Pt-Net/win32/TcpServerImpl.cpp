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
#include <Pt/Net/TcpServer.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/Selector.h>
#include <cerrno>
#include <cassert>
#include <cstring>

#define log_debug(x)

namespace Pt{
namespace Net{

TcpServerImpl::TcpServerImpl(TcpServer& server)
: _server(server)
, _fd(INVALID_SOCKET)
, _waitEvent(INVALID_HANDLE_VALUE)
{
}

void TcpServerImpl::create(int domain, int type, int protocol)
{
    log_debug("create socket");

    _fd = ::socket(domain, type, protocol);

    if (_fd == INVALID_SOCKET)
    {
        log_debug("Error at socket(): "<< WSAGetLastError());
        //freeaddrinfo(adrInfo);
        WSACleanup();
        throw System::SystemError( PT_ERROR_MSG("creating socket failed") );
    }

    _waitEvent = WSACreateEvent();
    attachEvent(_waitEvent, FD_CONNECT);
}

void TcpServerImpl::attachEvent(HANDLE ev, long events)
{
    if (WSAEventSelect(_fd, ev, events) == SOCKET_ERROR)
    {
        log_debug("Set event failed: "<< WSAGetLastError());
        throw System::SystemError( PT_ERROR_MSG("attach event to socket failed") );
    }
}

void TcpServerImpl::close()
{
    if (_fd == INVALID_SOCKET)
        return;

    WSACloseEvent(_waitEvent);
    _waitEvent = INVALID_HANDLE_VALUE;

    ::closesocket(_fd);
    _fd = INVALID_SOCKET;
    WSACleanup();
}

void TcpServerImpl::listen(const std::string& ipaddr, unsigned short int port, int backlog)
{
    log_debug("listen on " << ipaddr << " port " << port << " backlog " << backlog);

    Pt::System::MutexLock lock(_mutex);    

    //Initialize WSA
    if (WSAStartup(MAKEWORD(2,2), &_wsaData) != 0)
    {
        log_debug("WSAStartup failed");
        throw System::SystemError( PT_ERROR_MSG("initializing Winsocks failed") );
    }


    AddrInfo ai(ipaddr, port);

    BOOL reuseAddr = TRUE;

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

        // NOTE: set a socket option to reuse an address immediately after being closed
        // use WSA functions

        log_debug("setsockopt SO_REUSEADDR");

        if (::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddr, sizeof(reuseAddr)) < 0)
            throw System::SystemError("setsockopt");

        log_debug("bind");

        if (::bind(_fd, it->ai_addr, it->ai_addrlen) == 0)
        {
          // save our information
            std::memmove(&_servaddr, it->ai_addr, it->ai_addrlen);

            log_debug("listen");

            if (::listen(_fd, backlog) == SOCKET_ERROR)
            {
                ::closesocket(_fd);
                _fd = INVALID_SOCKET;
                WSACleanup();

                if (WSAGetLastError() == WSAEADDRINUSE)
                    throw AddressInUse();
                else
                    throw System::SystemError("listen");
            }

            return;
        }
    }

    throw System::SystemError("bind");
}

bool TcpServerImpl::wait(std::size_t msecs)
{
    log_debug("wait " << msecs);

    if(WSAWaitForMultipleEvents(1, &_waitEvent, FALSE, msecs, FALSE) != WSA_WAIT_TIMEOUT)
    {
        WSAResetEvent(_waitEvent);
        return true;
    }

    return false;
}

void TcpServerImpl::attach(System::SelectorBase& s)
{
    log_debug("attach to selector");
}

void TcpServerImpl::detach(System::SelectorBase& s)
{
    log_debug("detach from selector");
}

bool TcpServerImpl::setWaitHandle(HANDLE h, bool& avail)
{
    log_debug("setWaitHandle");

    attachEvent(h, FD_CONNECT);
    avail = true;

    return true;
}

void TcpServerImpl::getWaitHandles(System::HandleMap& handles, bool& avail)
{
    log_debug("getWaitHandles");
}

bool TcpServerImpl::checkEvent()
{
    log_debug("checkEvent");

    WSANETWORKEVENTS events;

    if(WSAEnumNetworkEvents(_fd, 0, &events) == SOCKET_ERROR)
        throw System::SystemError("ask network events failed");

    if((events.lNetworkEvents & FD_CONNECT) != FD_CONNECT)
        return false;

    _server.connectionPending.send(_server);
    return true;
}

} // namespace Net
} // namespace Pt
