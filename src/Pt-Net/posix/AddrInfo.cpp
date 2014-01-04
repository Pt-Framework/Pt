/*
 * Copyright (C) 2013 Marc Duerner
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
#include "EndpointImpl.h"
#include <Pt/Net/Endpoint.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>
#include <string>
#include <cstring>

log_define("Pt.Net.AddrInfo")

namespace Pt {

namespace Net {

AddrInfo::AddrInfo()
: _ai(0)
, _gainfo(0)
{
}


AddrInfo::~AddrInfo()
{
    clear();
}


void AddrInfo::resolve(const Endpoint& ep, bool passive)
{
    clear();

    const EndpointImpl* impl = ep.impl();

    const std::size_t addrlen = impl->addrlen();
    if(addrlen > 0)
    {
        const sockaddr* addr = impl->addr();
        std::memcpy(&_addr, addr, addrlen);
    
        _special.ai_family = _addr.ss_family;
        _special.ai_addr = reinterpret_cast<sockaddr*>(&_addr);
        _special.ai_addrlen = addrlen;
        _special.ai_next = 0;
    
        _ai = &_special;
        return;
    }

    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    
    // ai_socktype set to 0 does not work for QNX NTO
    hints.ai_socktype = SOCK_STREAM;

    if( passive )
        hints.ai_flags |= AI_PASSIVE;
    
    _host = impl->host();
    _service = impl->service();

    int error = ::getaddrinfo(_host.c_str(), _service.c_str(), &hints, &_gainfo);
    if(error)
    {
        log_error( "getaddrinfo: " << gai_strerror(error) );
        throw System::AccessFailed(_host + ':' + _service);
    }

    _ai = _gainfo;
}


void AddrInfo::clear()
{
    if(_gainfo)
    {
        freeaddrinfo(_gainfo);
        _gainfo = 0;
    }

    std::memset( &_special, 0, sizeof(_special) );
    std::memset( &_addr, 0, sizeof(_addr) );
    _ai = 0;
}


std::string AddrInfo::host()
{ 
    std::string str;

    if(_ai == &_special)
    {
        char addrStr[64] = {0};
        char serviceStr[64] = {0};

        socklen_t addrlen = static_cast<socklen_t>(_special.ai_addrlen);
        if( 0 == getnameinfo(_special.ai_addr, addrlen, addrStr, 64, serviceStr, 64, NI_NUMERICHOST) )
        {
            str += addrStr;
            str += ':';
            str += serviceStr;
        }
    }
    else
    {
        str += _host;
        str += ':';
        str += _service;
    }
    
    return str; 
}


void sockaddrToString(const sockaddr_storage& addr, std::string& str)
{
#ifdef WIN32

    const sockaddr* caddr = reinterpret_cast<const sockaddr*>(&addr);
    sockaddr* saddr = const_cast<sockaddr*>(caddr);
    
    DWORD len = 64; // length in characters, not bytes
    TCHAR adr[64];
    WSAAddressToString(saddr, sizeof(sockaddr), NULL, adr, &len);
    
    for(unsigned n = 0; n < len; n++)
        str.push_back( int(adr[n]) );

#else

    const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
    char strbuf[INET6_ADDRSTRLEN + 1];
    const char* p = inet_ntop(sa->sin_family, &sa->sin_addr, strbuf, sizeof(strbuf));
    str = (p == 0 ? "-" : strbuf);

#endif
}

} // namespace Net

} // namespace Pt
