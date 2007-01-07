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

#include "StreamSocketImpl.h"
#include <Pt/Exception.h>
#include <cerrno>


namespace Pt {

namespace Net {
/*
void StreamSocketImpl::connect(const std::string& ipaddr, unsigned short int port)
{
    // give some useful default values to use for getaddrinfo()
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

    AddrInfo ai(ipaddr, port, hints);

    for (AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        SocketImpl::create(it->ai_family, SOCK_STREAM, 0);

        if (::connect(handle(), it->ai_addr, it->ai_addrlen) == 0)
        {
            // save our information
            memmove(&peeraddr, it->ai_addr, it->ai_addrlen);
            return;
        }

        close();
    }

    throw Exception("connect", PT_SOURCEINFO);

}


size_t StreamSocketImpl::read(char* buffer, size_t count, bool& eof)
{
    ssize_t n;

    if (getTimeout() < 0)
    {
        // blocking read
        do
        {
			n = ::recv(handle(), buffer, count, 0);
        }
        while (n <= 0 && errno == EINTR);

        if (errno == WSAECONNRESET)
            eof = true;
        else if (n < 0)
            throw Exception("read", PT_SOURCEINFO); // TODO
    }
    else
    {
        // non-blocking read

        // try reading without timeout
        do
        {
            n = ::recv(handle(), buffer, count, 0);
        } 
        while (n <= 0 && errno == WSAEINTR);

        if (n <= 0)
        {
            // no data available

            if (errno == EAGAIN)
            {
                if (getTimeout() == 0)
                  return 0;

                this->wait(WaitInput, getTimeout());

                do
                {
                    n = ::recv(handle(), buffer, count, 0);
                } while (n <= 0 && errno == WSAEINTR);

                if (n < 0)
                    throw Exception("read", PT_SOURCEINFO); // TODO
            }
            else if (errno == WSAECONNRESET)
                eof = true;
            else if (errno != 0)
                throw Exception("read", PT_SOURCEINFO); // TODO
        }

    }

    return n < 0 ? 0 : n;

}


size_t StreamSocketImpl::write(const char* buffer, size_t count)
{
    ssize_t n = 0;
    size_t s = count;

    while (true)
    {
        do
        {
            n = ::send(handle(), buffer, s, 0);
        } while (n <= 0 && errno == EINTR);

        if (n <= 0)
        {
            if (errno == EAGAIN)
                n = 0;
            else
                throw Exception("write", PT_SOURCEINFO); // TODO
        }

        buffer += n;
        s -= n;

        if (s <= 0)
            break;

        if (getTimeout() == 0)
            return count - s;

        this->wait(WaitOutput, getTimeout());
    }

    return count;
}
*/


}
}
