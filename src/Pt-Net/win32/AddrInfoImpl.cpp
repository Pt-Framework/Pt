/*
 * Copyright (C) 2006-2009 by Marc Boris Duerner, Tommi Maekitalo
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
#include <sstream>
#include <string.h>

namespace Pt {

namespace Net {

AddrInfoImpl::AddrInfoImpl()
: _ai(0)
, _ainfo(0)
{
}


AddrInfoImpl::AddrInfoImpl(const std::string& ipaddr, unsigned short port, bool listen)
: _ai(0)
, _ainfo(0)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    if (listen)
        hints.ai_flags |= AI_PASSIVE;

    init(ipaddr, port, hints);
}


AddrInfoImpl::~AddrInfoImpl()
{
    clear();
}


AddrInfoImpl* AddrInfoImpl::ip4Any(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();
    impl->initIp4Any(port);
    return impl;
}


AddrInfoImpl* AddrInfoImpl::ip4Loopback(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();
    impl->initIp4Loopback(port);
    return impl;
}

AddrInfoImpl* AddrInfoImpl::ip4Broadcast(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();
    impl->initIp4Broadcast(port);
    return impl;
}


AddrInfoImpl* AddrInfoImpl::ip6Any(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();
    impl->initIp6Any(port);
    return impl;
}


AddrInfoImpl* AddrInfoImpl::ip6Loopback(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();
    impl->initIp6Loopback(port);
    return impl;
}


void AddrInfoImpl::clear()
{  
    if(_ainfo)
    {
        freeaddrinfo(_ainfo);
        _ainfo = 0;
    }

    memset( &_special, 0, sizeof(_special) );
    memset( &_specialAddr, 0, sizeof(_specialAddr) );
    _ai = 0;

    _host.clear();
    _port = 0;
}



void AddrInfoImpl::init(const std::string& ipaddr, unsigned short port, const addrinfo& hints)
{
    clear();
    
    std::ostringstream p;
    p << port;
    
    _host = ipaddr;
    _port = port;
    
    if( 0 != ::getaddrinfo(ipaddr.c_str(), p.str().c_str(), &hints, &_ainfo) )
        throw System::AccessFailed(_host + ':' + p.str());
    
    _ai = _ainfo;
}


void AddrInfoImpl::initIp4Any(unsigned short port)
{  
    clear();

    _port = port;

    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&_specialAddr);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;
    
    _special.ai_family = AF_INET;
    _special.ai_flags |= AI_PASSIVE;
    _special.ai_addr = (sockaddr*)(addr);
    _special.ai_addrlen = sizeof(sockaddr_in);
    _special.ai_next = 0;
    
    _ai = &_special;
}


void AddrInfoImpl::initIp4Loopback(unsigned short port)
{  
    clear();

    _port = port;

    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&_specialAddr);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_LOOPBACK;
    
    _special.ai_family = AF_INET;
    _special.ai_addr = (sockaddr*)(addr);
    _special.ai_addrlen = sizeof(sockaddr_in);
    _special.ai_next = 0;
    
    _ai = &_special;
}


void AddrInfoImpl::initIp4Broadcast(unsigned short port)
{  
    clear();

    _port = port;

    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&_specialAddr);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_BROADCAST;
    
    _special.ai_family = AF_INET;
    _special.ai_addr = (sockaddr*)(addr);
    _special.ai_addrlen = sizeof(sockaddr_in);
    _special.ai_next = 0;
    
    _ai = &_special;
}


void AddrInfoImpl::initIp6Any(unsigned short port)
{  
    clear();

    _port = port;

    sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(&_specialAddr);
    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);
    addr->sin6_addr = in6addr_any;
    
    _special.ai_family = AF_INET6;
    _special.ai_flags |= AI_PASSIVE;
    _special.ai_addr = (sockaddr*)(addr);
    _special.ai_addrlen = sizeof(sockaddr_in6);
    _special.ai_next = 0;
    
    _ai = &_special;
}


void AddrInfoImpl::initIp6Loopback(unsigned short port)
{  
    clear();

    _port = port;

    sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(&_specialAddr);
    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);
    addr->sin6_addr = in6addr_loopback;
    
    _special.ai_family = AF_INET6;
    _special.ai_addr = (sockaddr*)(addr);
    _special.ai_addrlen = sizeof(sockaddr_in6);
    _special.ai_next = 0;
    
    _ai = &_special;
}

}

}
