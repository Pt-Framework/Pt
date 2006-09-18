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

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Pt/Net/Ip4Address.h"

#include <iostream>
#include <string>
using namespace std;


namespace Pt {

namespace Net {

	bool Ip4Address::any() const throw()
	{
		return (*this == Ip4Address(Any));
	}


	bool Ip4Address::broadcast() const throw()
	{
		return (*this == Ip4Address(Broadcast));
	}


	bool Ip4Address::loopback() const throw()
	{
		return (*this == Ip4Address(Loopback));
	}

	std::vector<char> Ip4Address::data() const
	{
		std::vector<char> data(4);
		std::memcpy(&data[0], &_data, 4);
		return data;
	}

	std::string Ip4Address::str() const
	{
		return inet_ntoa((in_addr&)_data);
	}


	Ip4Address& Ip4Address::operator=(const Ip4Address& address)
	{
		_data = address._data;
		return *this;
	}


	Ip4Address& Ip4Address::operator=(const uint32_t address)
	{
		_data = address;
		return *this;
	}


	Ip4Address& Ip4Address::operator=(const std::string& str)
	{
		struct hostent* hent = ::gethostbyname( str.c_str() );
		if(hent) {
			if(hent->h_length != 4)
				throw Pt::Exception("Ip4Address: invalid address length.",PT_SOURCEINFO);

			(*this) = *( (uint32_t*)(hent->h_addr) );
			return (*this);
		}

		in_addr addr;
		inet_aton(str.c_str(), &addr);
		(*this) = (uint32_t&)addr;
		return (*this);
	}


	bool Ip4Address::operator==(const Ip4Address& addr) const
	{
		return _data == addr._data;
	}

}

}


