/***************************************************************************
 *   Copyright (C) 2004 by Marc Bpris Dürner                               *
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

#ifndef Pt_Net_NetworkAddress_h
#define Pt_Net_NetworkAddress_h
 
#include <Pt/Export.h>
#include <Pt/Types.h>
#include <string>
#include <vector>

class sockaddr;


namespace Pt {

namespace Net {

	typedef uint16_t port_t;


	class PT_API NetworkAddress {
		public:
			NetworkAddress()
			{}

			virtual ~NetworkAddress()
			{}

			virtual std::vector<char> data() const = 0;

			virtual std::string str() const = 0;
	};

} // !namespace Net

} // !namespace Pt

#endif
