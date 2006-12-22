/***************************************************************************
 *   Copyright (C) 2006 by Tommi Mäkitalo                                  *
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
 *   Copyright (C) 2006 by Stefan Büder                                    *
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

#include <Pt/Date.h>
#include <Pt/Exception.h>

#include <ctype.h>


namespace Pt
{
	/*namespace
	{
		inline bool isdigit(char ch)
		{ return ch >= '0' && ch <= '9'; }
	}*/

	std::string Date::toIsoString() const
	{
		// format YYYY-MM-DD
		//        0....+....1
		char ret[10];
		unsigned short n = _year;
		ret[3] = '0' + n % 10;
		n /= 10;
		ret[2] = '0' + n % 10;
		n /= 10;
		ret[1] = '0' + n % 10;
		n /= 10;
		ret[0] = '0' + n % 10;
		ret[4] = '-';
		ret[5] = '0' + _month / 10;
		ret[6] = '0' + _month % 10;
		ret[7] = '-';
		ret[8] = '0' + _day / 10;
		ret[9] = '0' + _day % 10;

		return std::string(ret, 10);
	}

	inline unsigned short getNumber2(const char* s)
	{
		if (!isdigit(s[0])
		|| !isdigit(s[1]))
		throw IllegalArgument("Illegal date format.", PT_SOURCEINFO);
		return (s[0] - '0') * 10
			+ (s[1] - '0');
	}

	inline unsigned short getNumber4(const char* s)
	{
		if (!isdigit(s[0])
		|| !isdigit(s[1])
		|| !isdigit(s[2])
		|| !isdigit(s[3]))
		throw IllegalArgument("Illegal date format.", PT_SOURCEINFO);
	  
		return (s[0] - '0') * 1000
			+ (s[1] - '0') * 100
			+ (s[2] - '0') * 10
			+ (s[3] - '0');
	}

	Date Date::fromIsoString(const std::string& s)
	{
		if (s.size() < 10
		    || s.at(4) != '-'
		    || s.at(7) != '-') {
			throw IllegalArgument("Illegal date format.", PT_SOURCEINFO);
		}
		
		const char* d = s.data();
		return Date(getNumber4(d), getNumber2(d + 5), getNumber2(d + 8));
	}

}
