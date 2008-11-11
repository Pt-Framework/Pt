/***************************************************************************
 *   Copyright (C) 2006 by Tommi Maekitalo                                 *
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
 *   Copyright (C) 2006 by Stefan Bueder                                   *
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
#include "Pt/Time.h"
#include "Pt/SerializationInfo.h"
#include <sstream>
#include <cctype>

namespace Pt {

InvalidTime::InvalidTime(const SourceInfo& si)
: std::invalid_argument("Invalid time" + si)
{
}


inline unsigned short getNumber2(const char* s)
{
    if ( !std::isdigit(s[0]) || !std::isdigit(s[1]) )
        throw InvalidTime(PT_SOURCEINFO);

    return (s[0] - '0') * 10 + (s[1] - '0');
}


inline unsigned short getNumber3(const char* s)
{
    if( !std::isdigit(s[0]) || !std::isdigit(s[1]) || !std::isdigit(s[2]) )
        throw InvalidTime(PT_SOURCEINFO);

    return ( s[0] - '0') * 100
             + (s[1] - '0') * 10
             + (s[2] - '0' );
}


void convert(Time& time, const std::string& s)
{
    unsigned hour = 0, min = 0, sec = 0, msec = 0;

    if( s.size() < 11 || s.at(2) != ':'
        || s.at(5) != ':' || s.at(8) != '.')
    {
        throw InvalidTime(PT_SOURCEINFO);
    }

    const char* d = s.data();
    hour = getNumber2(d);
	min = getNumber2(d + 3);
    sec = getNumber2(d + 6);
    msec = getNumber3(d + 9);
	
	time.set(hour, min, sec, msec);
}


void convert(std::string& str, const Time& time)
{
    unsigned hour = 0, minute = 0, second = 0, msec = 0;
    time.get(hour, minute, second, msec);

    // format hh:mm:ss.sssss
    //        0....+....1....+
    char ret[14];
    ret[0] = '0' + hour / 10;
    ret[1] = '0' + hour % 10;
    ret[2] = ':';
    ret[3] = '0' + minute / 10;
    ret[4] = '0' + minute % 10;
    ret[5] = ':';
    ret[6] = '0' + second / 10;
    ret[7] = '0' + second % 10;
    ret[8] = '.';
    unsigned short n = msec;
    ret[11] = '0' + n % 10;
    n /= 10;
    ret[10] = '0' + n % 10;
    n /= 10;
    ret[9] = '0' + n % 10;

    str.assign(ret, 12);
}


void operator >>=(const SerializationInfo& si, Time& time)
{
    std::string s = si.toValue<std::string>();
    convert(time, s);

    //unsigned hour = si.getValue<unsigned>("hour");
    //unsigned min = si.getValue<unsigned>("minute");
    //unsigned sec = si.getValue<unsigned>("second");
    //unsigned msec = si.getValue<unsigned>("millisec");
    //time.set(hour, min, sec, msec);
}


void operator <<=(SerializationInfo& si, const Time& time)
{
    std::string s;
    convert(s, time);
    si.setValue(s);
    si.setTypeName("Date");

    //unsigned hour = 0;
    //unsigned min = 0;
    //unsigned sec = 0;
    //unsigned msec = 0;
    //time.get(hour, min, sec, msec);

    //si.addValue("hour", hour );
    //si.addValue("minute", min );
    //si.addValue("second", sec );
    //si.addValue("millisec", msec );
    //si.setTypeName("Time");
}

} // namespace Pt
