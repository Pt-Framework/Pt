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

#include <Pt/Net/AddrInfo.h>
#include "AddrInfoImpl.h"
#include "TcpServerImpl.h"
#include <Pt/Net/TcpServer.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/Selector.h>
#include <cerrno>
#include <cassert>
#include <cstring>
#include <limits>

#define log_debug(x)

namespace Pt {

namespace Net {

static struct WsaInit
{
    WsaInit()
    { WSADATA wd; WSAStartup(MAKEWORD(2,2), &wd); }

    ~WsaInit()
    { WSACleanup(); }
} wsaInit;


TcpServerImpl::TcpServerImpl(TcpServer& server)
: _server(server)
, _fd(INVALID_SOCKET)
, _waitEvent(WSACreateEvent())
, _currentHandle(_waitEvent)
{
    ResetEvent(_currentHandle);
}

TcpServerImpl::~TcpServerImpl()
{
	WSAResetEvent(_waitEvent);
	close();

    WSACloseEvent(_waitEvent);
    _waitEvent = INVALID_HANDLE_VALUE;
}

void TcpServerImpl::create(int domain, int type, int protocol)
{
    log_debug("create socket");

    _fd = ::socket(domain, type, protocol);
	//_fd = WSASocket(domain, type, protocol, NULL , 0, 0);

    if (_fd == INVALID_SOCKET)
    {
        log_debug("Error at socket(): "<< WSAGetLastError());
        throw System::SystemError( PT_ERROR_MSG("creating socket failed") );
    }

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

    ::closesocket(_fd);
    _fd = INVALID_SOCKET;
}

void TcpServerImpl::listen(const std::string& ipaddr, unsigned short int port, int backlog)
{
    log_debug("listen on " << ipaddr << " port " << port << " backlog " << backlog);

    AddrInfo ai(ipaddr, port, true);

    BOOL reuseAddr = TRUE;

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

        // NOTE: set a socket option to reuse an address immediately after being closed

        // TODO: use WSA functions

        log_debug("setsockopt SO_REUSEADDR");

        if (::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddr, sizeof(reuseAddr)) < 0)
		{
			close();
            throw System::SystemError("setsockopt");
		}

        log_debug("bind");

        if (::bind(_fd, it->ai_addr, it->ai_addrlen) == 0)
        {
          // save our information
            std::memmove(&_servaddr, it->ai_addr, it->ai_addrlen);

            log_debug("listen");

            if (::listen(_fd, backlog) == SOCKET_ERROR)
            {
				close();

                if (WSAGetLastError() == WSAEADDRINUSE)
                    throw AddressInUse();
                else
                    throw System::SystemError("listen");
            }

            attachEvent(_currentHandle, FD_ACCEPT);

            return;
        }
    }

	close();
    throw System::SystemError("bind");
}

bool TcpServerImpl::wait(std::size_t umsecs)
{
    log_debug("wait " << msecs);

    // convert unsigned to signed
    int msecs = umsecs;
	if(umsecs == Pt::System::SelectorBase::WaitInfinite) 
    {
        msecs = INFINITE;
    }
    else if( umsecs > std::numeric_limits<int>::max() )
    {
        msecs = std::numeric_limits<int>::max();
    }

    if(WSAWaitForMultipleEvents(1, &_waitEvent, FALSE, msecs, FALSE) != WSA_WAIT_TIMEOUT)
    {
        WSAResetEvent(_waitEvent);
		_server.connectionPending.send(_server);
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

    if(_currentHandle == h)
        return true;

	_currentHandle = h;
    attachEvent(_currentHandle, FD_ACCEPT);
    avail = false;
    return true;
}

void TcpServerImpl::getWaitHandles(System::HandleMap& handles, bool& avail)
{
    log_debug("getWaitHandles");
}

HANDLE TcpServerImpl::waitHandle() const
{
    return _currentHandle;
}

bool TcpServerImpl::checkEvent()
{
    log_debug("checkEvent");

    WSANETWORKEVENTS events;

    if(WSAEnumNetworkEvents(_fd, _currentHandle, &events) == SOCKET_ERROR)
        throw System::SystemError("ask network events failed");

    if((events.lNetworkEvents & FD_ACCEPT) != FD_ACCEPT)
        return false;

    _server.connectionPending.send(_server);
    return true;
}

} // namespace Net

} // namespace Pt
