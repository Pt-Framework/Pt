/*
 * Copyright (C) 2005 Tommi Maekitalo
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
#ifndef PT_NET_ADDRINFOIMPL_H
#define PT_NET_ADDRINFOIMPL_H

#include <Pt/RefCounted.h>
#include <string>
#include <iterator>
#include <cassert>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>

namespace Pt {

namespace Net {

class AddrInfoImpl : public Pt::RefCounted
{
    public:
        class const_iterator : public std::iterator<std::forward_iterator_tag, addrinfo>
        {
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
                struct addrinfo* current;
        };

        AddrInfoImpl(const std::string& host, unsigned short port, bool listen);

        ~AddrInfoImpl();

        const std::string& host() const
        { return _host; }
        
        unsigned short port() const
        { return _port; }

        const_iterator begin() const  
        { return const_iterator(_ai); }

        const_iterator end() const    
        { return const_iterator(); }
        
        static AddrInfoImpl* anyIp4(unsigned short port); 

    protected:
        AddrInfoImpl();

        void init(const std::string& host, unsigned short port, const addrinfo& hints);

        void initIp4Any(unsigned short port);

        void initIp4Loopback(unsigned short port);

        void initIp4Broadcast(unsigned short port);

        void initIp6Any(unsigned short port);

        void initIp6Loopback(unsigned short port);

        void clear();

    private:
        std::string _host;
        unsigned short _port;
        struct addrinfo* _ai;
        struct addrinfo* _ainfo;
        struct addrinfo _special;
        struct sockaddr_storage _specialAddr;
};

void sockaddrToString(const sockaddr_storage& addr, std::string& str);

} // namespace Net

} // namespace Pt

#endif // PT_NET_ADDRINFOIMPL_H
