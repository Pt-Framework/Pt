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

#include "Pt/Net/Ip6Address.h"

#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
using namespace std;


namespace Pt {

namespace Net {

	const uint8_t Ip6Address::Any[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	const uint8_t Ip6Address::Loopback[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 };


	Ip6Address::Ip6Address(const uint8_t address[16])
	{
		std::memcpy(&_data, &address, 16);
	}


	Ip6Address::Ip6Address(const std::string& addr)
	{
		(*this) = addr;
	}


	std::vector<char> Ip6Address::data() const
	{
		std::vector<char> data(16);
		std::memcpy(&data[0], &_data, 16);
		return data;
	}


	std::string Ip6Address::str() const
	{
		in6_addr addr;
		std::memcpy(&addr, _data, 16);

		char str[INET6_ADDRSTRLEN];
		return inet_ntop(AF_INET6, &addr, str, INET6_ADDRSTRLEN);
	}


	Ip6Address& Ip6Address::operator=(const std::string& str)
	{
		in6_addr addr;
		std::memset(&addr, 0, 16);

		if( 0 != inet_pton(AF_INET6, str.c_str(), &addr) ) {
			std::memcpy(_data, &addr, 16);
		}
		else {
			std::memset(_data, 0, 16);
		}
	}

}

}

