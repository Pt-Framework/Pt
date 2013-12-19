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

#ifndef PT_NET_ENDPOINTIMPL_H
#define PT_NET_ENDPOINTIMPL_H

#include <Pt/Net/Api.h>
#include <Pt/WinVer.h>
#include <string>
#include <cassert>

#if defined(WIN32) || defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif

    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
#else
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <arpa/inet.h> // inet_ntop
    #include <netdb.h>
#endif

namespace Pt {

namespace Net {

class EndpointImpl
{
    public:
        EndpointImpl();

        EndpointImpl(const std::string& ipaddr, unsigned short port);

        EndpointImpl(const EndpointImpl& ipaddr);

        ~EndpointImpl();

        EndpointImpl& operator=(const EndpointImpl& ipaddr);

        void init(const sockaddr* addr, size_t addrlen);

        void clear();

        std::string toString() const;

        const std::string& host() const
        { return _host; }

        const std::string& service() const
        { return _service; }

        const ::sockaddr* addr() const
        { return reinterpret_cast<const sockaddr*>(&_addr); }

        size_t addrlen() const
        { return _addrlen; }

        static EndpointImpl* ip4Any(unsigned short port);

        static EndpointImpl* ip4Loopback(unsigned short port);

        static EndpointImpl* ip4Broadcast(unsigned short port);

        static EndpointImpl* ip6Any(unsigned short port);

        static EndpointImpl* ip6Loopback(unsigned short port);

    private:
        size_t _addrlen;
        ::sockaddr_storage _addr;
        std::string _host;
        std::string _service;
};

} // namespace Net

} // namespace Pt

#endif // PT_NET_ADDRINFOIMPL_H
