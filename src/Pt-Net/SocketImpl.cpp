/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Duerner, Tommi Maekitalo             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "SocketImpl.h"
#include <Pt/Net/Timeout.h>
#include <Pt/Exception.h>

//#include <winsock2.h>

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
