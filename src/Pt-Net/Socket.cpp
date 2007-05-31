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

#include "Pt/Net/Socket.h"
#include "SocketImpl.h"

#if defined(WIN32) || defined(_WIN32)
    #include <winsock2.h>
#else
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif


namespace Pt {

namespace Net {

unsigned long fromHost(unsigned long hostlong)
{
    return htonl(hostlong);
}

unsigned long toHost(unsigned long netlong)
{
    return ntohl(netlong);
}

unsigned int fromHost(unsigned int hostint)
{
    return htonl(hostint);
}

unsigned int toHost(unsigned int netint)
{
    return ntohl(netint);
}

unsigned short fromHost(unsigned short hostshort)
{
    return htons(hostshort);
}

unsigned short toHost(unsigned short netshort)
{
    return ntohs(netshort);
}

Socket::Socket()
{}

Socket::~Socket()
{}

} // namespace Net

} // namespace Pt
