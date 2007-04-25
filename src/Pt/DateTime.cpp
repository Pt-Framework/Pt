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
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <cassert>


namespace Pt {

inline double DateTime::toJulianDay(Timestamp::UtcTimeVal utcTime)
{
	double utcDays = double(utcTime)/864000000000.0;
	return utcDays + 2299160.5; // first day of Gregorian reform (Oct 15 1582)
}


inline Timestamp::UtcTimeVal DateTime::toUtcTime(double julianDay)
{
	return Timestamp::UtcTimeVal((julianDay - 2299160.5)*864000000000.0);
}


DateTime::DateTime()
{
	Timestamp now;
	_utcTime = now.utcTime();
	computeGregorian(julianDay());
	computeDaytime();
}


DateTime::DateTime(const Timestamp& timestamp):
	_utcTime(timestamp.utcTime())
{
	computeGregorian(julianDay());
	computeDaytime();
}

	
DateTime::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond):
	_year(year),
	_month(month),
	_day(day),
	_hour(hour),
	_minute(minute),
	_second(second),
	_millisecond(millisecond),
	_microsecond(microsecond)
{
	assert (year >= 0 && year <= 9999);
	assert (month >= 1 && month <= 12);
	assert (day >= 1 && day <= daysOfMonth(year, month));
	assert (hour >= 0 && hour <= 23);
	assert (minute >= 0 && minute <= 59);
	assert (second >= 0 && second <= 59);
	assert (millisecond >= 0 && millisecond <= 999);
	assert (microsecond >= 0 && microsecond <= 999);
	
	_utcTime = toUtcTime( toJulianDay(year, month, day)) + 10 *
	                    ( hour*Timespan::Hours +
                         minute * Timespan::Minutes +
                         second * Timespan::Seconds +
                         millisecond*Timespan::Milliseconds +
                         microsecond);
}


DateTime::DateTime(double julianDay):
	_utcTime(toUtcTime(julianDay))
{
	computeGregorian(julianDay);
}


DateTime::DateTime(Timestamp::UtcTimeVal utcTime, Timestamp::TimeDiff diff):
	_utcTime(utcTime + diff*10)
{
	computeGregorian(julianDay());
	computeDaytime();
}


DateTime::DateTime(const DateTime& dateTime):
	_utcTime(dateTime._utcTime),
	_year(dateTime._year),
	_month(dateTime._month),
	_day(dateTime._day),
	_hour(dateTime._hour),
	_minute(dateTime._minute),
	_second(dateTime._second),
	_millisecond(dateTime._millisecond),
	_microsecond(dateTime._microsecond)
{
}


DateTime::~DateTime()
{
}


DateTime& DateTime::operator = (const DateTime& dateTime)
{
	if (&dateTime != this)
	{
		_utcTime     = dateTime._utcTime;
		_year        = dateTime._year;
		_month       = dateTime._month;
		_day         = dateTime._day;
		_hour        = dateTime._hour;
		_minute      = dateTime._minute;
		_second      = dateTime._second;
		_millisecond = dateTime._millisecond;
		_microsecond = dateTime._microsecond;
	}
	return *this;
}

	
DateTime& DateTime::operator = (const Timestamp& timestamp)
{
	_utcTime = timestamp.utcTime();
	computeGregorian(julianDay());
	computeDaytime();
	return *this;
}


DateTime& DateTime::operator = (double julianDay)
{
	_utcTime = toUtcTime(julianDay);
	computeGregorian(julianDay);
	return *this;
}


DateTime& DateTime::assign(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
	assert (year >= 0 && year <= 9999);
	assert (month >= 1 && month <= 12);
	assert (day >= 1 && day <= daysOfMonth(year, month));
	assert (hour >= 0 && hour <= 23);
	assert (minute >= 0 && minute <= 59);
	assert (second >= 0 && second <= 59);
	assert (millisecond >= 0 && millisecond <= 999);
	assert (microsecond >= 0 && microsecond <= 999);

	_utcTime     = toUtcTime(toJulianDay(year, month, day)) + 10*(hour*Timespan::Hours + minute*Timespan::Minutes + second*Timespan::Seconds + millisecond*Timespan::Milliseconds + microsecond);
	_year        = year;
	_month       = month;
	_day         = day;
	_hour        = hour;
	_minute      = minute;
	_second      = second;
	_millisecond = millisecond;
	_microsecond = microsecond;
	
	return *this;
}


void DateTime::swap(DateTime& dateTime)
{
	std::swap(_utcTime, dateTime._utcTime);
	std::swap(_year, dateTime._year);
	std::swap(_month, dateTime._month);
	std::swap(_day, dateTime._day);
	std::swap(_hour, dateTime._hour);
	std::swap(_minute, dateTime._minute);
	std::swap(_second, dateTime._second);
	std::swap(_millisecond, dateTime._millisecond);
	std::swap(_microsecond, dateTime._microsecond);
}


int DateTime::dayOfWeek() const
{
	return int((floor(julianDay() + 1.5))) % 7;
}


int DateTime::dayOfYear() const
{
	int doy = 0;
	for (int month = 1; month < _month; ++month)
		doy += daysOfMonth(_year, month);
	doy += _day;
	return doy;
}


int DateTime::daysOfMonth(int year, int month)
{
	assert (month >= 1 && month <= 12);

	static int daysOfMonthTable[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	
	if (month == 2 && isLeapYear(year))
		return 29;
	else
		return daysOfMonthTable[month];
}


bool DateTime::isValid(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
	return
		(year >= 0 && year <= 9999) &&
		(month >= 1 && month <= 12) &&
		(day >= 1 && day <= daysOfMonth(year, month)) &&
		(hour >= 0 && hour <= 23) &&
		(minute >= 0 && minute <= 59) &&
		(second >= 0 && second <= 59) &&
		(millisecond >= 0 && millisecond <= 999) &&
		(microsecond >= 0 && microsecond <= 999);
}


int DateTime::week(int firstDayOfWeek) const
{
	assert (firstDayOfWeek >= 0 && firstDayOfWeek <= 6);

	/// find the first firstDayOfWeek.
	int baseDay = 1;
	while (DateTime(_year, 1, baseDay).dayOfWeek() != firstDayOfWeek) ++baseDay;

	int doy  = dayOfYear();
	int offs = baseDay <= 4 ? 0 : 1; 
	if (doy < baseDay)
		return offs;
	else
		return (doy - baseDay)/7 + 1 + offs;
}


double DateTime::julianDay() const
{
	return toJulianDay(_utcTime);
}


DateTime DateTime::operator + (const Timespan& span) const
{
	return DateTime(_utcTime, span.totalMicroseconds());
}


DateTime DateTime::operator - (const Timespan& span) const
{
	return DateTime(_utcTime, -span.totalMicroseconds());
}


Timespan DateTime::operator - (const DateTime& dateTime) const
{
	return Timespan((_utcTime - dateTime._utcTime)/10);
}


DateTime& DateTime::operator += (const Timespan& span)
{
	_utcTime += span.totalMicroseconds()*10;
	computeGregorian(julianDay());
	computeDaytime();
	return *this;
}


DateTime& DateTime::operator -= (const Timespan& span)
{
	_utcTime -= span.totalMicroseconds()*10;
	computeGregorian(julianDay());
	computeDaytime();
	return *this;
}


double DateTime::toJulianDay(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
	// lookup table for (153*month - 457)/5 - note that 3 <= month <= 14.
	static int lookup[] = {-91, -60, -30, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337};
 
	// day to double
	double dday = double(day) + ((double((hour*60 + minute)*60 + second)*1000 + millisecond)*1000 + microsecond)/86400000000.0;
	if (month < 3)
	{
		month += 12;
		--year;
	}
	double dyear = double(year);
	return dday + lookup[month] + 365*year + floor(dyear/4) - floor(dyear/100) + floor(dyear/400) + 1721118.5;
}


void DateTime::checkLimit(short& lower, short& higher, short limit)
{
	if (lower > limit)
	{
		higher += short(lower / limit);
		lower   = short(lower % limit);
	}
}


void DateTime::normalize()
{
	checkLimit(_microsecond, _millisecond, 999);
	checkLimit(_millisecond, _second, 999);
	checkLimit(_second, _minute, 59);
	checkLimit(_minute, _hour, 59);
	checkLimit(_hour, _day, 23);

	if (_day > daysOfMonth(_year, _month))
	{
		_day -= daysOfMonth(_year, _month);
		if (++_month > 12)
		{
			++_year;
			_month -= 12;
		}
	}
}


void DateTime::computeGregorian(double julianDay)
{
	double z    = floor(julianDay - 1721118.5);
	double r    = julianDay - 1721118.5 - z;
	double g    = z - 0.25;
	double a    = floor(g / 36524.25);
	double b    = a - floor(a/4);
	_year       = short(floor((b + g)/365.25));
	double c    = b + z - floor(365.25*_year);
	_month      = short(floor((5*c + 456)/153));
	double dday = c - floor((153.0*_month - 457)/5) + r;
	_day        = short(dday);
	if (_month > 12)
	{
		++_year;
		_month -= 12;
	}
	r      *= 24;
	_hour   = short(floor(r));
	r      -= floor(r);
	r      *= 60;
	_minute = short(floor(r));
	r      -= floor(r);
	r      *= 60;
	_second = short(floor(r));
	r      -= floor(r);
	r      *= 1000;
	_millisecond = short(floor(r));
	r      -= floor(r);
	r      *= 1000;
	_microsecond = short(r + 0.5);

	normalize();

	assert(_month >= 1 && _month <= 12);
	assert(_day >= 1 && _day <= daysOfMonth(_year, _month));
	assert(_hour >= 0 && _hour <= 23);
	assert(_minute >= 0 && _minute <= 59);
	assert(_second >= 0 && _second <= 59);
	assert(_millisecond >= 0 && _millisecond <= 999);
	assert(_microsecond >= 0 && _microsecond <= 999);
}


void DateTime::computeDaytime()
{
	Timespan span(_utcTime/10);
	_hour        = span.hours();
	_minute      = span.minutes();
	_second      = span.seconds();
	_millisecond = span.milliseconds();
	_microsecond = span.microseconds();
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
    unsigned short n = this->year();
    ret[3] = '0' + n % 10;
    n /= 10;
    ret[2] = '0' + n % 10;
    n /= 10;
    ret[1] = '0' + n % 10;
    n /= 10;
    ret[0] = '0' + n % 10;
    ret[4] = '-';
    ret[5] = '0' + this->month() / 10;
    ret[6] = '0' + this->month() % 10;
    ret[7] = '-';
    ret[8] = '0' + this->day() / 10;
    ret[9] = '0' + this->day() % 10;
    ret[10] = ' ';
    ret[11] = '0' + this->hour() / 10;
    ret[12] = '0' + this->hour() % 10;
    ret[13] = ':';
    ret[14] = '0' + this->minute() / 10;
    ret[15] = '0' + this->minute() % 10;
    ret[16] = ':';
    ret[17] = '0' + this->second() / 10;
    ret[18] = '0' + this->second() % 10;
    ret[19] = '.';
    n = this->millisecond();
    ret[22] = '0' + n % 10;
    n /= 10;
    ret[21] = '0' + n % 10;
    n /= 10;
    ret[20] = '0' + n % 10;

    return std::string(ret, 23);
}

/*

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
*/

}
