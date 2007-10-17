/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Duerner, Tommi Maekitalo             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_Net_AddrInfo_H
#define Pt_Net_AddrInfo_H

#include <Pt/Net/Api.h>

#if defined(WIN32) || defined(_WIN32)
    //#ifdef __GNUC__
    //#define WINVER 0x501
    //#endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
	#ifndef __GNUC__
//		#include <wspiapi.h>
	#endif
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
#endif

#include <Pt/Exception.h>
#include <iterator>
#include <string>
#include <sstream>


namespace Pt {

namespace Net {

    class AddrInfo
    {
        private:
            ::addrinfo* ai;

        public:  
            class const_iterator : public std::iterator<std::forward_iterator_tag, addrinfo>
            {
                private:
                    ::addrinfo* current;

                public:
                    explicit const_iterator(struct addrinfo* ai = 0)
                    : current(ai)
                    { }

                    bool operator== (const const_iterator& it) const
                    { return current == it.current; }

                    bool operator!= (const const_iterator& it) const
                    { return current != it.current; }

                    const_iterator& operator++ ()
                    { current = current->ai_next; return *this; }

                    const_iterator operator++ (int)
                    {
                      const_iterator ret(current);
                      current = current->ai_next;
                      return ret;
                    }

                    reference operator* () const
                    { return *current; }

                    pointer operator-> () const
                    { return current; }
            };

        public:
            AddrInfo(const std::string& ipaddr, unsigned short port, const addrinfo& hints);

            ~AddrInfo();  

            const_iterator begin() const  
            { return const_iterator(ai); }

            const_iterator end() const    
            { return const_iterator(); }
    };

}

}

#endif
