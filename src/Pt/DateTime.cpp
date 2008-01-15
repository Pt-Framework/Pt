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

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <cassert>


namespace Pt {

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

        
DateTime::DateTime(const Pt::int64_t milliSecondsSinceJan1st1970)
{
    // Init calculation variable to defaults.
    unsigned year = 1970;
    Pt::Date::Month mon = Pt::Date::Jan;
    unsigned day = 1;
    unsigned hour = 0;
    unsigned min = 0;
    unsigned secs = 0;
    // Cut the milliseconds part.
    Pt::int64_t msecs = milliSecondsSinceJan1st1970 % Pt::Time::MSECS_PER_SEC;
    // Check if we are before or beyond January 1st 1970.
    if(milliSecondsSinceJan1st1970 >= 0)
    {
        // Calculate the remaining milliseconds for the last day. (represents hours, minutes, seconds and milliseconds at least). 
        Pt::int64_t msecsRemain = milliSecondsSinceJan1st1970 % (Pt::Time::MSECS_PER_DAY);
        // Calculate the elapsed days since January 1st 1970.
        Pt::int64_t days = milliSecondsSinceJan1st1970 / (Pt::Time::MSECS_PER_DAY);
        
        // While there are more days than a year left increment the year counter and decrement the day counter. (Also consider leap years)
        bool cont = days > Pt::Date::DAYS_PER_YEAR;
        while(cont)
        {
            if(Pt::Date::leapYear(year))
            {
                days -= Pt::Date::DAYS_PER_LEAPYEAR;
            }
            else
            {
                days -= Pt::Date::DAYS_PER_YEAR;
            }
            year++;
            if(Pt::Date::leapYear(year))
            {
                cont = days >= Pt::Date::DAYS_PER_LEAPYEAR;
            }
            else
            {
                cont = days >= Pt::Date::DAYS_PER_YEAR;
            }
        }
        
        // Now we are in the appropriate year. So we can look for the appropriate month. (Also consider leap years) 
        std::map<Pt::Date::Month, Pt::uint8_t>::const_iterator it = monthMap().begin();
        for(; it != monthMap().end(); )
        {
            if(days > it->second)
            {
                if(mon == Pt::Date::Feb && Pt::Date::leapYear(year))
                {
                    days -= Pt::Date::DAYS_OF_LEAP_FEBUARY;
                }
                else
                {
                    days -= it->second;
                }
                it++;
                mon = it->first;
            }
            else
            {
                break;
            }
        }

        // Now we are at the appropriate day. So we can look for the appropriate time.
        hour = static_cast<unsigned>(msecsRemain  / (Pt::Time::MSECS_PER_HOUR));
        msecsRemain = msecsRemain % (Pt::Time::MSECS_PER_HOUR);
        min = static_cast<unsigned>(msecsRemain  / (Pt::Time::MSECS_PER_MIN));
        msecsRemain = msecsRemain % (Pt::Time::MSECS_PER_MIN);
        secs = static_cast<unsigned>(msecsRemain  / (Pt::Time::MSECS_PER_SEC));
        // Consider that we are one day ahead regarding the day counter.
        day = static_cast<unsigned>(days + 1);
    }
    else
    {
        // Calculate the remaining milliseconds for the first day. (represents hours, minutes, seconds and milliseconds at least). 
        Pt::int64_t msecsRemain = milliSecondsSinceJan1st1970 % (Pt::Time::MSECS_PER_DAY);
        // Calculate the days until January 1st 1970.
        Pt::int64_t days = milliSecondsSinceJan1st1970 / (Pt::Time::MSECS_PER_DAY);
        // Decrement the year counter since we are before 1970.
        year--;

        // While there are more days than a year left decrement the year counter and increment the day counter. (Also consider leap years)
        bool cont = days < -Pt::Date::DAYS_PER_YEAR;
        while(cont)
        {
            if(Pt::Date::leapYear(year))
            {
                days += Pt::Date::DAYS_PER_LEAPYEAR;
            }
            else
            {
                days += Pt::Date::DAYS_PER_YEAR;
            }
            year--;
            if(Pt::Date::leapYear(year))
            {
                cont = days <= -Pt::Date::DAYS_PER_LEAPYEAR;
            }
            else
            {
                cont = days <= -Pt::Date::DAYS_PER_YEAR;
            }
            if(year == 0 && cont)
            {
                throw std::logic_error("Dates before birth of Jesus Christ are not supported!");
            }
        }
        
        // Now we are in the appropriate year. So we can look for the appropriate month. (Also consider leap years) 
        std::map<Pt::Date::Month, Pt::uint8_t>::reverse_iterator it = monthMap().rbegin();
        for(; it != monthMap().rend(); )
        {
            if(days <= -(static_cast<Pt::int16_t>(it->second)))
            {                
                if(it->first == Pt::Date::Feb && Pt::Date::leapYear(year))
                {
                    days += Pt::Date::DAYS_OF_LEAP_FEBUARY;
                }
                else
                {
                    days += it->second;
                }
                it++;                    
            }
            else
            {
                mon = it->first;
                break;
            }
        }
        // If we have looped back to Dec of the previous year (too far) we go back to Jan of the following year.
        if(it == monthMap().rend())
        {
            mon = monthMap().rbegin()->first;
        }

        // Now we are at the appropriate day. So we can look for the appropriate time.
        hour = static_cast<unsigned>(Pt::Time::MAX_HOUR + msecsRemain  / (Pt::Time::MSECS_PER_HOUR));
        msecsRemain = msecsRemain % (Pt::Time::MSECS_PER_HOUR);
        min = static_cast<unsigned>(Pt::Time::MAX_MINUTE + msecsRemain  / (Pt::Time::MSECS_PER_MIN));
        msecsRemain = msecsRemain % (Pt::Time::MSECS_PER_MIN);
        secs = static_cast<unsigned>(Pt::Time::MAX_SECOND + msecsRemain  / (Pt::Time::MSECS_PER_SEC));
        // Now we calculate the day respecting leap years.
        if(mon == Pt::Date::Feb && Pt::Date::leapYear(year))
        {
            day = static_cast<unsigned>(Pt::Date::DAYS_OF_LEAP_FEBUARY + days);
        }
        else
        {
            day = static_cast<unsigned>(monthMap().find(mon)->second + days);
        }
        // We have to check if we have a day-underrun. In that case we have to increase the corresponding fields.
        if(msecs == 0)
        {
            if(secs == Pt::Time::MAX_SECOND)
            {
                secs = 0;
                if(min == Pt::Time::MAX_MINUTE)
                {
                    min = 0;
                    if(hour == Pt::Time::MAX_HOUR)
                    {
                        hour = 0;
                        std::map<Pt::Date::Month, Pt::uint8_t>::const_iterator tmpIt = monthMap().find(mon);
                        if(day == tmpIt->second)
                        {
                            day = 1;
                            tmpIt++;
                            if(tmpIt != monthMap().end())
                            {
                                mon = tmpIt->first;
                            }
                            else
                            {
                                mon = monthMap().begin()->first;
                            }
                        }
                        else
                        {
                            day++;
                        }
                    }
                    else
                    {
                        hour++;
                    }
                }
                else
                {
                    min++;
                }
            }
            else
            {
                secs++;
            }
        }
        else
        {
            msecs = Pt::Time::MSECS_PER_SEC + msecs;
        }
    }

    // We are done. Now construct the members for date and time.
    _date = Pt::Date(year, mon, day);
    _time = Pt::Time(hour, min, secs, static_cast<unsigned>(msecs));
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

Pt::int64_t DateTime::msecsSinceJan1st1970() const
{
    Pt::int64_t retVal = 0;
    // Check if we are before or beyond the reference date.
    if(date().year() >= 1970)
    {
        // First calculate the milliseconds elapsed in the current year.
        Pt::int32_t days = date().dayOfYear();
        retVal = days > 0 ? static_cast<Pt::int64_t>((days - 1)) * Pt::Time::MSECS_PER_DAY : 0;
        retVal += static_cast<Pt::int64_t>(time().hour()) * Pt::Time::MSECS_PER_HOUR;
        retVal += static_cast<Pt::int64_t>(time().minute()) * Pt::Time::MSECS_PER_MIN;
        retVal += static_cast<Pt::int64_t>(time().second()) * Pt::Time::MSECS_PER_SEC;
        retVal += static_cast<Pt::int64_t>(time().msec());

        // Reset the "days" variable for further calculation.
        days = 0;
        // Count the elapsed days since January 1st 1970 (also consider leap years).
        for(Pt::int32_t i = date().year() -1; i >= 1970; i--)
        {
            days += Pt::Date::leapYear(i) ? Pt::Date::DAYS_PER_LEAPYEAR : Pt::Date::DAYS_PER_YEAR;
        }
        // Add the milliseconds of the elapsed days to the return value.
        retVal += static_cast<Pt::int64_t>(days) * Pt::Time::MSECS_PER_DAY;
    }
    else
    {
        Pt::int32_t days;
        // First check if we are in a leap year and calculate the remaining days accordingly.
        if(Pt::Date::leapYear(date().year()))
        {
            days = Pt::Date::DAYS_PER_LEAPYEAR - date().dayOfYear();
        }
        else
        {
            days = Pt::Date::DAYS_PER_YEAR - date().dayOfYear();
        }
        // Calculate the milliseconds of the remaining days.
        retVal = days > 0 ? static_cast<Pt::int64_t>((days)) * Pt::Time::MSECS_PER_DAY : 0;
        retVal += static_cast<Pt::int64_t>((Pt::Time::HOURS_PER_DAY - time().hour() - 1)) * Pt::Time::MSECS_PER_HOUR;
        retVal += static_cast<Pt::int64_t>((Pt::Time::MINUTES_PER_HOUR - time().minute() - 1)) * Pt::Time::MSECS_PER_MIN;
        retVal += static_cast<Pt::int64_t>((Pt::Time::SECONDS_PER_MINUTE - time().second() - 1)) * Pt::Time::MSECS_PER_SEC;
        retVal += static_cast<Pt::int64_t>(Pt::Time::MSECS_PER_SEC - time().msec());

        // Reset the "days" variable for further calculation.
        days = 0;
        // Count the days before January 1st 1970 (also consider leap years).
        for(Pt::int32_t i = date().year() +1; i < 1970; i++)
        {
            days += Pt::Date::leapYear(i) ? Pt::Date::DAYS_PER_LEAPYEAR : Pt::Date::DAYS_PER_YEAR;
        }
        // Add the milliseconds of the days to the return value.
        retVal += static_cast<Pt::int64_t>(days) * Pt::Time::MSECS_PER_DAY;
        // Make the return value negative.
        retVal *= static_cast<Pt::int64_t>(-1);
    }
    return retVal;
}

std::map<Pt::Date::Month, Pt::uint8_t>& DateTime::monthMap() const
{
    static std::map<Pt::Date::Month, Pt::uint8_t> mMap;
    if(mMap.size() == 0)
    {
        mMap[Pt::Date::Jan] = Pt::Date::DAYS_OF_JANUARY;
        mMap[Pt::Date::Feb] = Pt::Date::DAYS_OF_FEBUARY;
        mMap[Pt::Date::Mar] = Pt::Date::DAYS_OF_MARCH;
        mMap[Pt::Date::Apr] = Pt::Date::DAYS_OF_APRIL;
        mMap[Pt::Date::May] = Pt::Date::DAYS_OF_MAY;
        mMap[Pt::Date::Jun] = Pt::Date::DAYS_OF_JUNE;
        mMap[Pt::Date::Jul] = Pt::Date::DAYS_OF_JULY;
        mMap[Pt::Date::Aug] = Pt::Date::DAYS_OF_AUGUST;
        mMap[Pt::Date::Sep] = Pt::Date::DAYS_OF_SEPTEMBER;
        mMap[Pt::Date::Oct] = Pt::Date::DAYS_OF_OCTOBER;
        mMap[Pt::Date::Nov] = Pt::Date::DAYS_OF_NOVEMBER;
        mMap[Pt::Date::Dec] = Pt::Date::DAYS_OF_DECEMBER;
    }
    return mMap;
}

}
