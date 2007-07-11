/***************************************************************************
 *   Copyright (C) 2006 by Tommi Mäkitalo                                  *
 *   Copyright (C) 2006 by Marc Boris Duerner                               *
 *   Copyright (C) 2006 by Stefan Bueder                                    *
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

#include <Pt/DateTime.h>
#include "Pt/SourceInfo.h"
#include "Pt/SerializationData.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <cassert>


namespace Pt {

/*
const Archive& operator>>(const Archive& ar, DateTime& dt)
{
    ar >> dt._date;
    ar >> dt._time;

    return ar;
}


Archive& operator<<(Archive& ar, const DateTime& dt)
{
    ar << dt._date;
    ar << dt._time;

    return ar;
}*/


DateTime::DateTime()
{
}


DateTime::DateTime(int year, unsigned month, unsigned day,
                   unsigned hour, unsigned minute, unsigned second, unsigned msec)
: _date(year, month, day)
, _time(hour, minute, second, msec)
{
}


DateTime::DateTime(unsigned julianDay)
: _date(julianDay)
{
}


DateTime::DateTime(const DateTime& dateTime)
: _date( dateTime.date() )
, _time( dateTime.time() )
{
}


DateTime::~DateTime()
{
}


DateTime& DateTime::operator=(const DateTime& dateTime)
{
    _date = dateTime.date();
    _time = dateTime.time();
    return *this;
}


DateTime& DateTime::operator=(unsigned julianDay)
{
    _time = Time(0, 0, 0, 0);
    _date.setJulian(julianDay);
    return *this;
}


void DateTime::set(int year, unsigned month, unsigned day,
                   unsigned hour, unsigned minute, unsigned second, unsigned msec)
{
    _date.set(year, month, day);
    _time.set(hour, minute, second, msec);
}


void DateTime::get(int& y, unsigned& month, unsigned& d,
                   unsigned& h, unsigned& min, unsigned& s, unsigned& ms) const
{
    _date.get(y, month, d);
    _time.get(h, min, s, ms);
}


bool DateTime::isValid(int year, unsigned month, unsigned day,
                       unsigned hour, unsigned minute, unsigned second, unsigned msec)
{
    return Date::isValid(year, month, day) && Time::isValid(hour, minute, second, msec);
}


inline unsigned short getNumber2(const char* s)
{
    if (!std::isdigit(s[0])
        || !std::isdigit(s[1]))
        throw std::invalid_argument("Not a digit." + PT_SOURCEINFO);
    return (s[0] - '0') * 10
        + (s[1] - '0');
}

inline unsigned short getNumber3(const char* s)
{
    if (!std::isdigit(s[0])
        || !std::isdigit(s[1])
        || !std::isdigit(s[2]))
        throw std::invalid_argument("Not a digit." + PT_SOURCEINFO);
    return (s[0] - '0') * 100
        + (s[1] - '0') * 10
        + (s[2] - '0');
}

inline unsigned short getNumber4(const char* s)
{
    if (!std::isdigit(s[0])
        || !std::isdigit(s[1])
        || !std::isdigit(s[2])
        || !std::isdigit(s[3]))
        throw std::invalid_argument("Not a digit." + PT_SOURCEINFO);
    return (s[0] - '0') * 1000
        + (s[1] - '0') * 100
        + (s[2] - '0') * 10
        + (s[3] - '0');
}


DateTime DateTime::fromIsoString(const std::string& s)
{
    if (s.size() < 23
        || s.at(4) != '-'
        || s.at(7) != '-'
        || s.at(10) != ' '
        || s.at(13) != ':'
        || s.at(16) != ':'
        || s.at(19) != '.')
        throw std::invalid_argument("Invalid date-time iso string" + PT_SOURCEINFO);

    const char* d = s.data();

    return DateTime( getNumber4(d),
                     getNumber2(d + 5),
                     getNumber2(d + 8),
                     getNumber2(d + 11),
                     getNumber2(d + 14),
                     getNumber2(d + 17),
                     getNumber3(d + 20) );
}


std::string DateTime::toIsoString() const
{
    // format YYYY-MM-DD hh:mm:ss.sssss
    //        0....+....1....+....2....+
    char ret[25];
    unsigned short n = this->date().year();
    ret[3] = '0' + n % 10;
    n /= 10;
    ret[2] = '0' + n % 10;
    n /= 10;
    ret[1] = '0' + n % 10;
    n /= 10;
    ret[0] = '0' + n % 10;
    ret[4] = '-';
    ret[5] = '0' + this->date().month() / 10;
    ret[6] = '0' + this->date().month() % 10;
    ret[7] = '-';
    ret[8] = '0' + this->date().day() / 10;
    ret[9] = '0' + this->date().day() % 10;
    ret[10] = ' ';
    ret[11] = '0' + this->time().hour() / 10;
    ret[12] = '0' + this->time().hour() % 10;
    ret[13] = ':';
    ret[14] = '0' + this->time().minute() / 10;
    ret[15] = '0' + this->time().minute() % 10;
    ret[16] = ':';
    ret[17] = '0' + this->time().second() / 10;
    ret[18] = '0' + this->time().second() % 10;
    ret[19] = '.';
    n = this->time().msec();
    ret[22] = '0' + n % 10;
    n /= 10;
    ret[21] = '0' + n % 10;
    n /= 10;
    ret[20] = '0' + n % 10;

    return std::string(ret, 23);
}



const SerializationData& operator>>(const SerializationData& data, DateTime& datetime)
{
    const SerializationData* subData = data.getData(L"date");
    if(!subData)
        throw NoSuchEntry("date", PT_SOURCEINFO);

    Date date(1,1,1);
    *subData >> date;
    datetime.setDate(date);

    subData = data.getData(L"time");
    if(!subData)
        throw NoSuchEntry("time", PT_SOURCEINFO);

    Time time;
    *subData >> time;
    datetime.setTime(time);

    return data;
}


SerializationData& operator<<(SerializationData& data, const DateTime& datetime)
{
    SerializationData& dateData = data.addData(L"date");
    dateData << datetime.date();

    SerializationData& timeData = data.addData(L"time");
    timeData << datetime.time();

    return data;
}


}
