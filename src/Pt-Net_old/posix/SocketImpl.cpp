/*
 * Copyright (C) 2006 by Marc Boris Duerner, Tommi Maekitalo
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

#include "SocketImpl.h"
#include <Pt/Net/Timeout.h>
#include <Pt/Exception.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>


namespace Pt {

namespace Net {


SocketImpl::~SocketImpl()
{
    if (fd >= 0)
        close();
}


void SocketImpl::create(int domain, int type, int protocol)
{
    fd = ::socket(domain, type, protocol);
    if (fd < 0)
      throw RuntimeError("cannot create socket", PT_SOURCEINFO); // TODO change exceptiontype
}


void SocketImpl::close()
{
    ::close(fd);
    //closesocket(fd);
    fd = -1;
}


short SocketImpl::doPoll(short events, int timeout) const
{
    struct pollfd fds;
    fds.fd = getFd();
    fds.events = events;

    int p = ::poll(&fds, 1, timeout);

    if (p < 0)
      throw Exception("poll", PT_SOURCEINFO); // TODO
    else if (p == 0)
      throw Timeout();
    else if (fds.revents & (POLLERR | POLLHUP | POLLNVAL))
      throw Exception("poll", PT_SOURCEINFO); // TODO

    return fds.revents;
}


} // namespace Net

} // namespace Pt
