/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
 *   Copyright (C) 2004 by Christian Prochnow                              *
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

#ifndef _Pt_Net_Ip6Address_h
#define _Pt_Net_Ip6Address_h

#include <Pt/Export.h>
#include <Pt/Net/NetworkAddress.h>


namespace Pt {

namespace Net {

	class PT_EXPORT Ip6Address : public NetworkAddress {
		public:
			static const uint8_t Any[16];
			static const uint8_t Loopback[16];

		public:
			Ip6Address(const uint8_t address[16] = Any);
			Ip6Address(const std::string& address);

			std::vector<char> data() const;

			std::string str() const;

		public:
			Ip6Address& operator=(const std::string& str);

		private:
			uint8_t _data[16];
	};

} // !namespace Net

} // !namespace Pt

#endif
