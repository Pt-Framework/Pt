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

namespace Pt {

namespace Net {

TcpServerImpl::TcpServerImpl(TcpServer& server)
: _server(server)
// NOTE: _handle(INVALID_HANDLE_VALUE)
{

}


void TcpServerImpl::create(int domain, int type, int protocol)
{
    log_debug("create socket");

    // NOTE: initialise WSA -> thread safety !!!
    // NOTE: create WSA socket
}


void TcpServerImpl::close()
{
    // NOTE: close WSA handle if not INVALID_HANDLE_VALUE

    //if (_fd >= 0)
    //{
    //    log_debug("close socket");
    //    ::close(_fd);
    //    _fd = -1;
    //}
}


void TcpServerImpl::listen(const std::string& ipaddr, unsigned short int port, int backlog)
{
  log_debug("listen on " << ipaddr << " port " << port << " backlog " << backlog);

  AddrInfo ai(ipaddr, port);

  int reuseAddr = 1;

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

    //log_debug("setsockopt SO_REUSEADDR");
    //if (::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) < 0)
    //  throw System::SystemError("setsockopt");

    //log_debug("bind");
    //if (::bind(_fd, it->ai_addr, it->ai_addrlen) == 0)
    //{
    //  // save our information
    //    std::memmove(&servaddr, it->ai_addr, it->ai_addrlen);
    //
    //    log_debug("listen");
    //    if (::listen(_fd, backlog) < 0)
    //    {
    //        if (errno == EADDRINUSE)
    //            throw AddressInUse();
    //        else
    //            throw System::SystemError("listen");
    //    }
    //
    //    return;
    //}
  }

  throw System::SystemError("bind");
}


bool TcpServerImpl::wait(std::size_t msecs)
{
    log_debug("wait " << msecs);

    // NOTE: wait for acivity without a Selector
    // use WaitForSingleObject here. Note that this method could be
    // called while we are also in a Selector.

    // We probably need to keep our own Event object here.
    // Btw, WSAEvent is the same like normal win32 Event objects

    return false; // true if we became active
}


void TcpServerImpl::attach(System::SelectorBase& s)
{
    log_debug("attach to selector");

    // NOTE: called when we are added to the Selector. This always happens
    // after listen() was called.
}


void TcpServerImpl::detach(System::SelectorBase& s)
{
    log_debug("detach from selector");

    // NOTE: called when we are removed from a Selector.
}


bool TcpServerImpl::setWaitHandle(HANDLE h, bool& avail)
{
    log_debug("setWaitHandle");

    // NOTE: HANDLE h is the handle used in the Selector for WaitForMultipleObjects
    // we need to set avail to true if we are immediately ready.

    // Use WSAEventSelect with the HANDLE to be notified of new connections.

    // Btw, WSAEvent is the same like normal win32 Event objects

    // return true to show that we use HANDLE h
    // this means the Selector will not call getWaitHandles()
    return true;
}


void TcpServerImpl::getWaitHandles(System::HandleMap& handles, bool& avail)
{
    log_debug("getWaitHandles");

    // NOTE: not needed.
}


bool TcpServerImpl::checkEvent()
{
    log_debug("checkEvent");

    // NOTE: check that really something is available...

    _server.connectionPending.send(_server);
    return true;
}

} // namespace Net

} // namespace Pt
