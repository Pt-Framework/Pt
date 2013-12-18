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

#include "EndpointImpl.h"
#include <Pt/Net/Endpoint.h>
#include <Pt/System/IOError.h>
#include <string>
#include <sstream>
#include <cstring>

namespace Pt {

namespace Net {

EndpointImpl::EndpointImpl()
: _addrlen(0)
{
}


EndpointImpl::EndpointImpl(const std::string& ipaddr, unsigned short port)
: _addrlen(0)
, _host(ipaddr)
{
    std::stringstream ss;
    ss << port;
    ss >> _service;
}


EndpointImpl::EndpointImpl(const EndpointImpl& ep)
: _addrlen(0)
, _host(ep._host)
, _service(ep._service)
{
    if(ep._addrlen)
    {
        std::memcpy(&_addr, ep.addr(), ep._addrlen);
        _addrlen = ep._addrlen;
    }
}


EndpointImpl::~EndpointImpl()
{
}


void EndpointImpl::clear()
{  
    _addrlen = 0;
    _host.clear();
    _service.clear();
}


void EndpointImpl::init(const sockaddr* addr, std::size_t addrlen)
{  
    clear();

    std::memcpy(&_addr, addr, addrlen);
    _addrlen = addrlen;
}


EndpointImpl& EndpointImpl::operator=(const EndpointImpl& ainfo)
{
    _addrlen = 0;
    _host = ainfo._host;
    _service = ainfo._service;

    if(ainfo._addrlen)
    {      
        std::memcpy(&_addr, ainfo.addr(), ainfo._addrlen);
        _addrlen = ainfo._addrlen;
    }
    
    return *this;
}


std::string EndpointImpl::toString() const
{ 
    std::string str;

    if(_addrlen > 0)
    {
        char addrStr[64] = {0};
        char serviceStr[64] = {0};

        if( 0 == getnameinfo(addr(), static_cast<socklen_t>(_addrlen), addrStr, 64, serviceStr, 64, NI_NUMERICHOST) )
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


EndpointImpl* EndpointImpl::ip4Any(unsigned short port)
{
    EndpointImpl* impl = new EndpointImpl();

    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&impl->_addr);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = htonl(INADDR_ANY);

    impl->_addrlen = sizeof(sockaddr_in);

    return impl;
}


EndpointImpl* EndpointImpl::ip4Loopback(unsigned short port)
{
    EndpointImpl* impl = new EndpointImpl();

    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&impl->_addr);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    impl->_addrlen = sizeof(sockaddr_in);

    return impl;
}


EndpointImpl* EndpointImpl::ip4Broadcast(unsigned short port)
{
    EndpointImpl* impl = new EndpointImpl();

    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&impl->_addr);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = htonl(INADDR_BROADCAST);
    
    impl->_addrlen = sizeof(sockaddr_in);

    return impl;
}


EndpointImpl* EndpointImpl::ip6Any(unsigned short port)
{
    EndpointImpl* impl = new EndpointImpl();

    sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(&impl->_addr);
    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);
    addr->sin6_addr = in6addr_any;
    
    impl->_addrlen = sizeof(sockaddr_in6);

    return impl;
}


EndpointImpl* EndpointImpl::ip6Loopback(unsigned short port)
{
    EndpointImpl* impl = new EndpointImpl();

    sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(&impl->_addr);
    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);
    addr->sin6_addr = in6addr_loopback;
    
    impl->_addrlen = sizeof(sockaddr_in6);

    return impl;
}

} // namespace Net

} // namespace Pt
