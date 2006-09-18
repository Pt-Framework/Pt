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

#ifndef _Pt_Net_Ip4Address_h
#define _Pt_Net_Ip4Address_h

#include <Pt/Export.h>
#include <Pt/Net/NetworkAddress.h>


namespace Pt {

namespace Net {

		class PT_EXPORT Ip4Address : public NetworkAddress {
			public:
				enum Type {
					Any        = 0x00000000,
					Broadcast  = 0xffffffff,
					Loopback   = 0x7f000001
				};

			public:
				Ip4Address(const uint32_t address = Any)
				{ (*this) = address; }

				Ip4Address(const std::string& address)
				{ (*this) = address; }

				bool any() const throw();

				bool broadcast() const throw();

				bool loopback() const throw();

				const uint32_t& address() const
				{ return _data; }

				std::vector<char> data() const;

				std::string str() const;

			public:
				Ip4Address& operator=(const Ip4Address& address);
				Ip4Address& operator=(const uint32_t address);
				Ip4Address& operator=(const std::string& str);
				bool operator==(const Ip4Address& addr) const;
	
			private:
				uint32_t _data;
		};

} // !namespace Net

} // !namespace Pt

#endif
