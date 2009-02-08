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

#include "TcpSocketImpl.h"
#include "AddrInfo.h"
#include <Pt/Exception.h>
#include <errno.h>
#include <sys/poll.h>

namespace Pt
{
namespace Net
{

void TcpSocketImpl::connect(const std::string& ipaddr, unsigned short int port)
{
    // give some useful default values to use for getaddrinfo()
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

    AddrInfo ai(ipaddr, port, hints);

    for (AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        SocketImpl::create(it->ai_family, SOCK_STREAM, 0);

        if (::connect(getFd(), it->ai_addr, it->ai_addrlen) == 0)
        {
            // save our information
            memmove(&peeraddr, it->ai_addr, it->ai_addrlen);
            return;
        }

        close();
    }

    throw Exception("connect", PT_SOURCEINFO);

}

size_t TcpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
    ssize_t n;

    if (getTimeout() < 0)
    {
        // blocking read
        do
        {
            n = ::read(getFd(), buffer, count);
        } while (n <= 0 && errno == EINTR);

        if (errno == ECONNRESET)
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
            n = ::read(getFd(), buffer, count);
        } while (n <= 0 && errno == EINTR);

        if (n <= 0)
        {
            // no data available

            if (errno == EAGAIN)
            {
                if (getTimeout() == 0)
                  return 0;

                doPoll(POLLIN, getTimeout());

                do
                {
                    n = ::read(getFd(), buffer, count);
                } while (n <= 0 && errno == EINTR);

                if (n < 0)
                    throw Exception("read", PT_SOURCEINFO); // TODO
            }
            else if (errno == ECONNRESET)
                eof = true;
            else if (errno != 0)
                throw Exception("read", PT_SOURCEINFO); // TODO
        }

    }

    return n < 0 ? 0 : n;

}

size_t TcpSocketImpl::write(const char* buffer, size_t count)
{
    ssize_t n = 0;
    size_t s = count;

    while (true)
    {
        do
        {
            n = ::write(getFd(), buffer, s);
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

        doPoll(POLLOUT, getTimeout());
    }

    return count;
}



}
}
