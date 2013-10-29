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

#include "AddrInfoImpl.h"
#include <Pt/Net/AddrInfo.h>
#include <Pt/System/IOError.h>
#include <string>
#include <sstream>
#include <iostream>
#include <cstring>

namespace Pt {

namespace Net {

EndpointImpl::EndpointImpl()
: _addrlen(0)
, _listen(false)
{
}


EndpointImpl::EndpointImpl(const std::string& ipaddr, unsigned short port, bool listen)
: _addrlen(0)
, _host(ipaddr)
, _listen(listen)
{
    std::stringstream ss;
    ss << port;
    ss >> _service;
}


EndpointImpl::EndpointImpl(const EndpointImpl& ainfo)
: _addrlen(0)
, _host(ainfo._host)
, _service(ainfo._service)
, _listen(ainfo._listen)
{
    if(ainfo._addrlen)
    {
        memcpy(&_addr, ainfo.addr(), ainfo._addrlen);
        _addrlen = ainfo._addrlen;
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
    _listen = false;
}


void EndpointImpl::init(const sockaddr* addr, size_t addrlen)
{  
    clear();

    memcpy(&_addr, addr, addrlen);
    _addrlen = addrlen;
}


EndpointImpl& EndpointImpl::operator=(const EndpointImpl& ainfo)
{
    _addrlen = 0;
    _host = ainfo._host;
    _service = ainfo._service;
    _listen = ainfo._listen;

    if(ainfo._addrlen)
    {      
        memcpy(&_addr, ainfo.addr(), ainfo._addrlen);
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

        if( 0 == getnameinfo(addr(), _addrlen, addrStr, 64, serviceStr, 64, NI_NUMERICHOST) )
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
    addr->sin_addr.s_addr = INADDR_ANY;

    impl->_addrlen = sizeof(sockaddr_in);

    return impl;
}


EndpointImpl* EndpointImpl::ip4Loopback(unsigned short port)
{
    EndpointImpl* impl = new EndpointImpl();

    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&impl->_addr);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_LOOPBACK;

    impl->_addrlen = sizeof(sockaddr_in);

    return impl;
}


EndpointImpl* EndpointImpl::ip4Broadcast(unsigned short port)
{
    EndpointImpl* impl = new EndpointImpl();

    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&impl->_addr);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_BROADCAST;
    
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




AddrInfo::AddrInfo()
: _ai(0)
, _gainfo(0)
{
}


AddrInfo::~AddrInfo()
{
    clear();
}


void AddrInfo::resolve(const Endpoint& ep)
{
    clear();

    const EndpointImpl* impl = ep.impl();

    const size_t addrlen = impl->addrlen();
    if(addrlen > 0)
    {
        const sockaddr* addr = impl->addr();
        memcpy(&_addr, addr, addrlen);
    
        _special.ai_family = _addr.ss_family;
        _special.ai_addr = reinterpret_cast<sockaddr*>(&_addr);
        _special.ai_addrlen = addrlen;
        _special.ai_next = 0;
    
        _ai = &_special;
        return;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    if( impl->isListen() )
        hints.ai_flags |= AI_PASSIVE;
    
    _host = impl->host();
    _service = impl->service();

    if( 0 != ::getaddrinfo(_host.c_str(), _service.c_str(), &hints, &_gainfo) )
        throw System::AccessFailed(_host + ':' + _service);

    _ai = _gainfo;
}


void AddrInfo::clear()
{
    if(_gainfo)
    {
        freeaddrinfo(_gainfo);
        _gainfo = 0;
    }

    memset( &_special, 0, sizeof(_special) );
    memset( &_addr, 0, sizeof(_addr) );
    _ai = 0;
}


std::string AddrInfo::host()
{ 
    std::string str;

    if(_ai == &_special)
    {
        char addrStr[64] = {0};
        char serviceStr[64] = {0};

        if( 0 == getnameinfo(_special.ai_addr, _special.ai_addrlen, addrStr, 64, serviceStr, 64, NI_NUMERICHOST) )
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
