/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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

#ifndef Pt_Net_TcpSocket_h
#define Pt_Net_TcpSocket_h
 
#include <Pt/Export.h>
#include <Pt/Net/Socket.h>

#include <iostream>


namespace Pt {

namespace Net {

	class PT_API TcpSocket : virtual public Socket {
		public:
			TcpSocket()
			{}

			virtual ~TcpSocket() throw()
			{ }

			virtual void listen() throw(IO::IOError) = 0;
	};


	class PT_API TcpServerSocket : virtual public ServerSocket {
		public:
			TcpServerSocket()
			{}

			virtual void listen() throw(IO::IOError) = 0;
	};
} // !namespace Net

} // !namespace Pt

#endif
