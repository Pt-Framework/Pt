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

#ifndef Pt_Net_SocketImpl_h
#define Pt_Net_SocketImpl_h

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

namespace Pt
{
namespace Net
{
    class SocketImpl
    {
        public:
            SocketImpl() : fd(-1) { }
            ~SocketImpl();
            void create(int domain, int type, int protocol);
            void close();
            short doPoll(short events, int timeout) const;

        protected:
            int getFd() const    { return fd; }

        private:
            int fd;
    };
}
}

#endif
