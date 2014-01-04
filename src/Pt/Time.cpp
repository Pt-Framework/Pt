/*
 * Copyright (C) 2006 by Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Pt/Time.h>
#include <Pt/Convert.h>
#include <Pt/SerializationInfo.h>
#include <cctype>

namespace Pt {

InvalidTime::InvalidTime()
: std::runtime_error("invalid time")
{ }

void operator>>=(const SerializationInfo& si, Time& time)
{
    unsigned hour = 0;
    unsigned min = 0;
    unsigned sec = 0;
    unsigned msec = 0;

    if( si.compose(time) )
        return;

    si.getMember("hour") >>=  hour;
    si.getMember("min") >>= min;
    si.getMember("sec") >>=  sec;
    si.getMember("msec") >>=  msec;
    time.set(hour, min, sec, msec);
}


void operator<<=(SerializationInfo& si, const Time& time)
{
    if( si.decompose(time) )
        return;

    unsigned hour = 0;
    unsigned min = 0;
    unsigned sec = 0;
    unsigned msec = 0;
    time.get(hour, min, sec, msec);

    si.addMember( Pt::LiteralPtr<char>("hour") ) <<= static_cast<uint16_t>(hour);
    si.addMember( Pt::LiteralPtr<char>("min") ) <<= static_cast<uint16_t>(min);
    si.addMember( Pt::LiteralPtr<char>("sec") ) <<= static_cast<uint16_t>(sec);
    si.addMember( Pt::LiteralPtr<char>("msec") ) <<= static_cast<uint16_t>(msec);
    si.setTypeName( Pt::LiteralPtr<char>("Pt::Time") );
}


inline unsigned short getNumber2(const Char* s)
{
    if ( ! isdigit(s[0]) || ! isdigit(s[1]) )
        throw ConversionError("invalid time");

    return (s[0].value() - '0') * 10 + (s[1].value() - '0');
}


inline unsigned short getNumber3(const Char* s)
{
    if( ! isdigit(s[0]) || ! isdigit(s[1]) || ! isdigit(s[2]) )
       throw ConversionError("invalid time");

    return ( s[0].value() - '0') * 100 + (s[1].value() - '0') * 10 + (s[2].value() - '0' );
}


inline unsigned short getNumber2(const char* s)
{
    if ( ! std::isdigit(s[0]) || ! std::isdigit(s[1]) )
        throw ConversionError("invalid time");

    return (s[0] - '0') * 10 + (s[1] - '0');
}


inline unsigned short getNumber3(const char* s)
{
    if( ! std::isdigit(s[0]) || ! std::isdigit(s[1]) || ! std::isdigit(s[2]) )
       throw ConversionError("invalid time");

    return ( s[0] - '0') * 100 + (s[1] - '0') * 10 + (s[2] - '0' );
}


template <typename CharT>
void timeToString(std::basic_string<CharT>& str, const Time& time)
{
    unsigned hour = 0, minute = 0, second = 0, msec = 0;
    time.get(hour, minute, second, msec);

    // format hh:mm:ss.sssss
    //        0....+....1....+
    CharT ret[14];
    ret[0] = static_cast<char>('0' + hour / 10);
    ret[1] = '0' + hour % 10;
    ret[2] = ':';
    ret[3] = static_cast<char>('0' + minute / 10);
    ret[4] = '0' + minute % 10;
    ret[5] = ':';
    ret[6] = static_cast<char>('0' + second / 10);
    ret[7] = '0' + second % 10;
    ret[8] = '.';
    unsigned int n = msec;
    ret[11] = '0' + n % 10;
    n /= 10;
    ret[10] = '0' + n % 10;
    n /= 10;
    ret[9] = '0' + n % 10;

    str.assign(ret, 12);
}


std::string timeToString(const Time& time)
{
    std::string str;
    timeToString(str, time);
    return str;
}

Time timeFromString(const std::string& s)
{
    unsigned hour = 0, min = 0, sec = 0, msec = 0;

    if( s.size() < 11 || s.at(2) != ':' || s.at(5) != ':' || s.at(8) != '.')
        throw ConversionError("invalid time");

	const char* d = s.data();
	hour = getNumber2(d);
	min = getNumber2(d + 3);
	sec = getNumber2(d + 6);
	msec = getNumber3(d + 9);

	return Time(hour, min, sec, msec);
}

} // namespace Pt
