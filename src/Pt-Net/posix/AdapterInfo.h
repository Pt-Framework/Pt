/*
 * Copyright (C) 2014 Marc Duerner
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

#ifndef PT_NET_ADAPTERINFO_H
#define PT_NET_ADAPTERINFO_H

#include <Pt/Net/api.h>

// currently unused:
//#define PT_NET_WITH_IFCONF
//#define PT_NET_WITH_GETIFADDRS

#include <sys/ioctl.h> 
#include <net/if.h>

#ifdef PT_NET_WITH_GETIFADDRS
#include <ifaddrs.h>
#endif

namespace Pt {

namespace Net {

#ifdef PT_NET_WITH_GETIFADDRS

class AdapterInfo
{
    public:
        class Iterator
        {
            public:
                explicit Iterator(ifaddrs* ifa = 0);
                
                bool operator== (const Iterator& it) const
                { return _current == it._current; }

                bool operator!= (const Iterator& it) const
                { return _current != it._current; }

                Iterator& operator++ ();

                const char* operator*() const;

                int index();

                sockaddr* addr();

            private:
                ifaddrs* _current;
        };
    
    public:
        AdapterInfo(int sock);

        ~AdapterInfo();

        Iterator begin()
        { return Iterator(_adapters); }

        Iterator end()
        { return Iterator(); }

    private:
        ifaddrs* _adapters;
};

#elif defined(PT_NET_WITH_IFCONF)

class AdapterInfo
{
    public:
        class Iterator
        {
            public:
                explicit Iterator(int sock = 0, ifconf* ifc = 0, ifreq* ifr = 0);

                bool operator== (const Iterator& it) const
                { return _current == it._current; }

                bool operator!= (const Iterator& it) const
                { return _current != it._current; }

                Iterator& operator++();

                const char* operator*() const;

                int index();

                sockaddr* addr();

            private:
                int _sock;
                ifconf* _ifc;
                int     _offset;
                ifreq*  _current;
                ifreq   _ifr;
        };
    
    public:
        AdapterInfo(int sock);

        ~AdapterInfo();

        Iterator begin()
        { return Iterator(_sock, &_ifc, _ifr); }

        Iterator end()
        { return Iterator(); }

    private:
        int _sock;
        ifreq  _ifr[20];
        ifconf _ifc;
};

#else

class AdapterInfo
{
    public:
        class Iterator
        {
            public:
                explicit Iterator(struct if_nameindex* ifaces = 0, int sock = 0);

                bool operator== (const Iterator& it) const
                { return _current == it._current; }

                bool operator!= (const Iterator& it) const
                { return _current != it._current; }

                Iterator& operator++ ();

                const char* operator*() const;

                int index();

                sockaddr* addr();

            private:
                int _sock;
                struct if_nameindex* _current;
                struct ifreq _ifr;
        };
    
    public:
        AdapterInfo(int sock);

        ~AdapterInfo();

        Iterator begin()
        { return Iterator(_ifaces, _sock); }

        Iterator end()
        { return Iterator(); }

    private:
        int _sock;
        struct if_nameindex* _ifaces;
};

#endif

} // namespace Net

} // namespace Pt

#endif // PT_NET_ADAPTERINFO_H
