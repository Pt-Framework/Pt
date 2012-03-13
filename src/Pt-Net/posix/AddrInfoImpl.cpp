/*
 * Copyright (C) 2003,2009 Tommi Maekitalo
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

#include "AddrInfoImpl.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SystemError.h"
#include <string>
#include <sstream>
#include <cstring>
#include <arpa/inet.h>

namespace Pt {

namespace Net {

AddrInfoImpl::AddrInfoImpl(const std::string& host, unsigned short port, bool listen)
: _ai(0)
{ 
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));

    if (listen)
        hints.ai_flags |= AI_PASSIVE;

    init(host, port, hints); 
}


AddrInfoImpl::~AddrInfoImpl()
{
    if (_ai)
        freeaddrinfo(_ai);
}


void AddrInfoImpl::init(const std::string& host, unsigned short port, const addrinfo& hints)
{
    if (_ai)
    {
        freeaddrinfo(_ai);
        _ai = 0;
    }

    _host = host;
    _port = port;

    std::ostringstream p;
    p << port;

    const char* node = 0;

    if( ! host.empty() )
        node = host.c_str();

    if (0 != ::getaddrinfo(node, p.str().c_str(), &hints, &_ai))
        throw System::AccessFailed(_host + ':' + p.str());
}


void sockaddrToString(const sockaddr_storage& addr, std::string& str)
{
#ifdef PT_WITH_INET_NTOA
    static Pt::System::Mutex monitor;
    Pt::System::MutexLock lock(monitor);

    const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
    const char* p = inet_ntoa(sa->sin_addr);
    if (p)
        str = p;
    else
        str.clear();
#else
    const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
    char strbuf[INET6_ADDRSTRLEN + 1];
    const char* p = inet_ntop(sa->sin_family, &sa->sin_addr, strbuf, sizeof(strbuf));
    str = (p == 0 ? "-" : strbuf);
#endif
}

} // namespace Net

} // namespace Pt
