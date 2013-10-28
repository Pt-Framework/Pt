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
#include <iostream>
#include <cstring>
#include <arpa/inet.h> // inet_ntop
#include <netdb.h> // getifaddr

namespace Pt {

namespace Net {

AddrInfoImpl::AddrInfoImpl()
: _addrlen(0)
, _port(0)
, _listen(false)
{
}


AddrInfoImpl::AddrInfoImpl(const std::string& ipaddr, unsigned short port, bool listen)
: _addrlen(0)
, _host(ipaddr)
, _port(port)
, _listen(listen)
{
}


AddrInfoImpl::AddrInfoImpl(const AddrInfoImpl& ainfo)
: _addrlen(0)
, _host(ainfo._host)
, _port(ainfo._port)
, _listen(ainfo._listen)
{
    if(ainfo._addrlen)
    {
        memcpy(&_addr, ainfo.addr(), ainfo._addrlen);
        _addrlen = ainfo._addrlen;
    }
}


AddrInfoImpl::~AddrInfoImpl()
{
}


void AddrInfoImpl::clear()
{  
    _addrlen = 0;
    _host.clear();
    _port = 0;
    _listen = false;
}


void AddrInfoImpl::init(const sockaddr* addr, size_t addrlen)
{  
    clear();

    memcpy(&_addr, addr, addrlen);
    _addrlen = addrlen;
}


AddrInfoImpl& AddrInfoImpl::operator=(const AddrInfoImpl& ainfo)
{
    _addrlen = 0;
    _host = ainfo._host;
    _port = ainfo._port;
    _listen = ainfo._listen;

    if(ainfo._addrlen)
    {      
        memcpy(&_addr, ainfo.addr(), ainfo._addrlen);
        _addrlen = ainfo._addrlen;
    }
    
    return *this;
}


std::string AddrInfoImpl::host() const
{ 
    if(_addrlen > 0)
    {
        char addrStr[32] = {0};
        char serviceStr[32] = {0};

        if( 0 == getnameinfo(addr(), _addrlen, addrStr, 32, serviceStr, 32, NI_NUMERICHOST) )
        {
            return addrStr;
        }
    }

    return _host; 
}


AddrInfoImpl* AddrInfoImpl::ip4Any(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();

    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&impl->_addr);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;

    impl->_addrlen = sizeof(sockaddr_in);
    impl->_port = port;

    return impl;
}


AddrInfoImpl* AddrInfoImpl::ip4Loopback(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();

    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&impl->_addr);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_LOOPBACK;

    impl->_addrlen = sizeof(sockaddr_in);
    impl->_port = port;

    return impl;
}


AddrInfoImpl* AddrInfoImpl::ip4Broadcast(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();

    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&impl->_addr);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_BROADCAST;
    
    impl->_addrlen = sizeof(sockaddr_in);
    impl->_port = port;

    return impl;
}


AddrInfoImpl* AddrInfoImpl::ip6Any(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();

    sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(&impl->_addr);
    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);
    addr->sin6_addr = in6addr_any;
    
    impl->_addrlen = sizeof(sockaddr_in6);
    impl->_port = port;

    return impl;
}


AddrInfoImpl* AddrInfoImpl::ip6Loopback(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();

    sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(&impl->_addr);
    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);
    addr->sin6_addr = in6addr_loopback;
    
    impl->_addrlen = sizeof(sockaddr_in6);
    impl->_port = port;

    return impl;
}




Resolver::Resolver()
: _ai(0)
, _gainfo(0)
{
}


Resolver::~Resolver()
{
    clear();
}


void Resolver::resolve(const AddrInfoImpl& ai)
{
    clear();

    const sockaddr* addr = ai.addr();
    if(addr)
    {
        memcpy(&_addr, addr, ai.addrlen());
    
        _special.ai_family = _addr.ss_family;
        _special.ai_addr = reinterpret_cast<sockaddr*>(&_addr);
        _special.ai_addrlen = ai.addrlen();
        _special.ai_next = 0;
    
        _ai = &_special;
        return;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    if( ai.isListen() )
        hints.ai_flags |= AI_PASSIVE;
    
    std::ostringstream p;
    p << ai.port();
    
    if( 0 != ::getaddrinfo(ai.host().c_str(), p.str().c_str(), &hints, &_gainfo) )
        throw System::AccessFailed(ai.host() + ':' + p.str());
    
    _host = ai.host();
    _ai = _gainfo;
}


void Resolver::clear()
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


std::string Resolver::host()
{ 
    if(_ai == &_special)
    {
        char addrStr[32] = {0};
        char serviceStr[32] = {0};

        if( 0 == getnameinfo(_special.ai_addr, _special.ai_addrlen, addrStr, 32, serviceStr, 32, NI_NUMERICHOST) )
        {
            return std::string(addrStr) + ":" + serviceStr;
        }
    }

    return _host; 
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
