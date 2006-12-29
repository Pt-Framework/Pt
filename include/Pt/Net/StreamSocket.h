/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Dürner, Tommi Maekitalo              *
 *                                                                         *
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

#ifndef Pt_Net_StreamSocket_h
#define Pt_Net_StreamSocket_h

#include <Pt/Net/Socket.h>


namespace Pt
{

namespace Net
{

    class StreamSocketImpl;

    class PT_API StreamSocket : public Socket
    {
        public:
            StreamSocket()
              : _impl(0)
              { }

            StreamSocket(const std::string& ipaddr, unsigned short int port)
              : _impl(0)
              { connect(ipaddr, port); }

            void setTimeout(ssize_t msec);
            void connect(const std::string& ipaddr, unsigned short int port);

        protected:
            size_t _read(char* buffer, size_t count, bool& eof);
		    size_t _write(const char* buffer, size_t count);

        private:
            StreamSocketImpl* _impl;
    };

} // !namespace Net

} // !namespace Pt

#endif
