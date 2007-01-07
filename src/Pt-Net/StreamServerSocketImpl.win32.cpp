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

#include "StreamServerSocketImpl.h"
#include "StreamSocketImpl.h"
#include <Pt/Exception.h>
#include <cerrno>


namespace Pt {

namespace Net {

StreamServerSocketImpl::~StreamServerSocketImpl()
{
}


void StreamServerSocketImpl::bind(const std::string& ipaddr, unsigned short int port)
{
    // give some useful default values to use for getaddrinfo()
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

    AddrInfo ai(ipaddr, port, hints);

    for (AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        SocketImpl::create(it->ai_family, SOCK_STREAM, 0);

        //if ( ::connect(getFd(), it->ai_addr, it->ai_addrlen) == 0 )
        SOCKET fd = ::bind(handle(), it->ai_addr, it->ai_addrlen);
        if ( fd == 0 ) {
            // save our information
            setHandle(fd);
            return;
        }

        this->close();
    }

    throw Exception("connect", PT_SOURCEINFO); //TODO: Exception
}


void StreamServerSocketImpl::listen(unsigned backlog)
{
	int ret = ::listen(this->handle(), backlog);
	if(ret == -1)
		throw Exception("Could not listen on socket", PT_SOURCEINFO); //TODO: Exception
}


StreamSocketImpl* StreamServerSocketImpl::accept()
{
  struct sockaddr_storage peeraddr;
  socklen_t peeraddr_len;
  peeraddr_len = sizeof(peeraddr);
  SOCKET fd = ::accept(handle(), reinterpret_cast <struct sockaddr *> (&peeraddr), &peeraddr_len);

  if (fd < 0)
    throw Exception("accept", PT_SOURCEINFO); // TODO

  return new StreamSocketImpl(fd, peeraddr);
}

} // namespace Net

} // namespace Pt
