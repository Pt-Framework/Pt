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

#ifndef PT_NET_ADDRINFO_H
#define PT_NET_ADDRINFO_H

#include <Pt/WinVer.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <iterator>
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

class Endpoint;

class AddrInfo : private NonCopyable
{
    public:  
        class const_iterator
        {
            public:
                typedef ::addrinfo value_type;
                typedef std::ptrdiff_t difference_type;
                typedef std::forward_iterator_tag iterator_category;
                typedef const ::addrinfo* pointer;
                typedef const ::addrinfo& reference;

            public:
                explicit const_iterator(struct addrinfo* ai = 0)
                : current(ai)
                { }

                bool operator== (const const_iterator& it) const
                { return current == it.current; }

                bool operator!= (const const_iterator& it) const
                { return current != it.current; }

                const_iterator& operator++ ()
                { 
                    assert(current);
                    current = current->ai_next; 
                    return *this; 
                }

                const_iterator operator++ (int)
                {
                    assert(current);
                    const_iterator ret(current);
                    current = current->ai_next;
                    return ret;
                }

                reference operator* () const
                { 
                    assert(current); 
                    return *current; 
                }

                pointer operator-> () const
                { 
                    assert(current); 
                    return current; 
                }

            private:
                ::addrinfo* current;
        };

    public:
        AddrInfo();

        ~AddrInfo();

        void resolve(const Endpoint& ep, bool passive = false);

        void clear();

        std::string host();

        const_iterator begin() const
        { return const_iterator(_ai); }

        const_iterator end() const
        { return const_iterator(); }

    private:
        ::addrinfo* _ai;
        ::addrinfo* _gainfo;
        ::addrinfo _special;
        ::sockaddr_storage _addr;

        // TODO: place hostname string in sockaddr_storage and addrinfo
        std::string _host;
        std::string _service;
};

void sockaddrToString(const sockaddr_storage& addr, std::string& str);

} // namespace Net

} // namespace Pt

#endif // PT_NET_ADDRINFOIMPL_H
