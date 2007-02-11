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

#include <Pt/DateTime.h>
#include <Pt/Exception.h>

#include <ctype.h>


namespace Pt
{
    /*namespace
    {
        inline bool isdigit(char ch)
        { return ch >= '0' && ch <= '9'; }
    }*/

    std::string DateTime::toIsoString() const
    {
        // format YYYY-MM-DD hh:mm:ss.sssss
        //        0....+....1....+....2....+
        char ret[25];
        unsigned short n = this->years();
        ret[3] = '0' + n % 10;
        n /= 10;
        ret[2] = '0' + n % 10;
        n /= 10;
        ret[1] = '0' + n % 10;
        n /= 10;
        ret[0] = '0' + n % 10;
        ret[4] = '-';
        ret[5] = '0' + this->months() / 10;
        ret[6] = '0' + this->months() % 10;
        ret[7] = '-';
        ret[8] = '0' + this->days() / 10;
        ret[9] = '0' + this->days() % 10;
        ret[10] = ' ';
        ret[11] = '0' + this->hours() / 10;
        ret[12] = '0' + this->hours() % 10;
        ret[13] = ':';
        ret[14] = '0' + this->minutes() / 10;
        ret[15] = '0' + this->minutes() % 10;
        ret[16] = ':';
        ret[17] = '0' + this->seconds() / 10;
        ret[18] = '0' + this->seconds() % 10;
        ret[19] = '.';
        n = this->msecs();
        ret[22] = '0' + n % 10;
        n /= 10;
        ret[21] = '0' + n % 10;
        n /= 10;
        ret[20] = '0' + n % 10;

        return std::string(ret, 23);
    }

    inline unsigned short getNumber2(const char* s)
    {
        if (!isdigit(s[0])
            || !isdigit(s[1]))
            throw std::invalid_argument("Not a digit." + PT_SOURCEINFO);
        return (s[0] - '0') * 10
            + (s[1] - '0');
    }

    inline unsigned short getNumber3(const char* s)
    {
        if (!isdigit(s[0])
            || !isdigit(s[1])
            || !isdigit(s[2]))
            throw std::invalid_argument("Not a digit." + PT_SOURCEINFO);
        return (s[0] - '0') * 100
            + (s[1] - '0') * 10
            + (s[2] - '0');
    }

    inline unsigned short getNumber4(const char* s)
    {
        if (!isdigit(s[0])
            || !isdigit(s[1])
            || !isdigit(s[2])
            || !isdigit(s[3]))
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
        
        Date date( getNumber4(d), getNumber2(d + 5), getNumber2(d + 8) );
        Time time( getNumber2(d + 11), getNumber2(d + 14), getNumber2(d + 17), getNumber3(d + 20) );
        
        return DateTime( date, time );
    }

}
