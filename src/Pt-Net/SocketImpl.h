/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Duerner, Tommi Maekitalo             *
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

#ifdef WIN32
	#include <winsock2.h>
	#define SHUT_RDWR 2
#else
	typedef int SOCKET;
#endif

namespace Pt {

namespace Net {

    class SocketImpl
    {
		public:
			enum WaitMode
			{
				WaitInput = 0x1, WaitOutput = 0x2
			};

        public:
            SocketImpl();

            ~SocketImpl();

            void create(int domain, int type, int protocol);

            void close();

            bool wait(WaitMode events, int timeout) const;

        protected:
            SOCKET handle() const
						{ return _sd; }

        private:
            SOCKET _sd;
    };

} // namespace Net

} //namespace Pt

#endif
