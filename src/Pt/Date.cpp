/*
 * Copyright (C) 2004-2008 Marc Boris Duerner
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

#include "Pt/Date.h"
#include "Pt/Convert.h"
#include "Pt/SerializationInfo.h"
#include "Pt/SerializationContext.h"
#include <cctype>

namespace Pt {

InvalidDate::InvalidDate()
: std::runtime_error("Invalid date")
{
}


void greg2jul(unsigned& jd, int y, int m, int d)
{
    if( ! Date::isValid(y, m, d) )
    {
        throw InvalidDate();
    }

    jd=(1461*(y+4800+(m-14)/12))/4+(367*(m-2-12*((m-14)/12)))/12-(3*((y+4900+(m-14)/12)/100))/4+d-32075;
}


void jul2greg(unsigned jd, int& y, int& m, int& d)
{
    register int l, n, i, j;
    l = jd + 68569;
    n = (4*l) / 146097;
    l = l - (146097*n+3) / 4;
    i = (4000 * (l+1)) / 1461001;
    l = l - (1461*i) / 4+31;
    j = (80*l) / 2447;
    d = l - (2447*j) / 80;
    l = j / 11;
    m = j + 2 - (12*l);
    y = 100 * (n-49) + i + l;
}


inline unsigned short getNumber2(const Char* s)
{
    if( ! isdigit(s[0]) 
     || ! isdigit(s[1]) )
    throw ConversionError("Invalid date format.");
    return (s[0].value() - '0') * 10
         + (s[1].value() - '0');
}


inline unsigned short getNumber4(const Char* s)
{
    if ( ! isdigit(s[0]) 
      || ! isdigit(s[1])
      || ! isdigit(s[2])  
      || ! isdigit(s[3]) )
    throw ConversionError("Invalid date format.");

    return (s[0].value() - '0') * 1000
         + (s[1].value() - '0') * 100
         + (s[2].value() - '0') * 10
         + (s[3].value() - '0');
}


inline unsigned short getNumber2(const char* s)
{
    if( ! std::isdigit(static_cast<unsigned char>(s[0])) 
     || ! std::isdigit(static_cast<unsigned char>(s[1])) )
    throw ConversionError("Invalid date format.");
    return (s[0] - '0') * 10
        + (s[1] - '0');
}


inline unsigned short getNumber4(const char* s)
{
    if ( ! std::isdigit(static_cast<unsigned char>(s[0])) 
      || ! std::isdigit(static_cast<unsigned char>(s[1]))
      || ! std::isdigit(static_cast<unsigned char>(s[2]))  
      || ! std::isdigit(static_cast<unsigned char>(s[3])) )
    throw ConversionError("Invalid date format.");

    return (s[0] - '0') * 1000
        + (s[1] - '0') * 100
        + (s[2] - '0') * 10
        + (s[3] - '0');
}


template <typename CharT>
void dateToString(std::basic_string<CharT>& str, const Date& date)
{
    // format YYYY-MM-DD
    //        0....+....1

    int year, month, day;
    jul2greg(date.julian(), year, month, day);

    CharT ret[10];
    unsigned int n = year;

    ret[3] = '0' + n % 10;
    n /= 10;
    ret[2] = '0' + n % 10;
    n /= 10;
    ret[1] = '0' + n % 10;
    n /= 10;
    ret[0] = '0' + n % 10;
    ret[4] = '-';
    ret[5] = static_cast<char>('0' + month / 10);
    ret[6] = '0' + month % 10;
    ret[7] = '-';
    ret[8] = static_cast<char>('0' + day / 10);
    ret[9] = '0' + day % 10;

    str.assign(ret, 10);
}


std::string dateToString(const Date& date)
{
    std::string str;
    dateToString(str, date);
    return str;
}


Date dateFromString(const std::string& s)
{
    if( s.size() < 10 || s.at(4) != '-' || s.at(7) != '-' )
        throw ConversionError("Illegal date format");

    const char* d = s.data();
    return Date(getNumber4(d), getNumber2(d + 5), getNumber2(d + 8));
}


void operator>>=(const SerializationInfo& si, Date& date)
{
    int year = 0;
    unsigned month = 0, day = 0;

    if( si.compose(date) )
        return;

    si.getMember("year") >>=  year;
    si.getMember("month") >>= month;
    si.getMember("day") >>=  day;
    date.set(year, month, day);
}


void operator<<=(SerializationInfo& si, const Date& date)
{
    if( si.decompose(date) )
        return;

    si.addMember( Pt::LiteralPtr<char>("year") ) <<= static_cast<int32_t>( date.year() );
    si.addMember( Pt::LiteralPtr<char>("month") ) <<= static_cast<uint16_t>( date.month() );
    si.addMember( Pt::LiteralPtr<char>("day") ) <<= static_cast<uint16_t>( date.day() );
    si.setTypeName( Pt::LiteralPtr<char>("Pt::Date") );
}

} // namespace Pt
