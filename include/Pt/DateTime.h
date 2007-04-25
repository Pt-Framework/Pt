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

#ifndef PT_DATETIME_H
#define PT_DATETIME_H

#include <Pt/Api.h>
#include <Pt/Time.h>
#include <Pt/Date.h>
#include "Pt/Timestamp.h"
#include "Pt/Timespan.h"

#include <string>
#include <limits>


namespace Pt {

/** This class represents an instant in time, expressed
    in years, months, days, hours, minutes, seconds
    and milliseconds based on the Gregorian calendar.
    The class is mainly useful for conversions between
    UTC, Julian day and Gregorian calendar dates.

    Conversion calculations are based on algorithms
    collected and described by Peter Baum at
    http://vsg.cape.com/~pbaum/date/date0.htm

    Internally, this class stores a date/time in two
    forms (UTC and broken down) for performance reasons. Only use
    this class for conversions between date/time representations.
    Use the Timestamp class for everything else.

    Notes:
    * Zero is a valid year (in accordance with ISO 8601 and astronomical year numbering)
    * Year zero (0) is a leap year
    * Negative years (years preceding 1 BC) are not supported

    For more information, please see:
    * http://en.wikipedia.org/wiki/Gregorian_Calendar
    * http://en.wikipedia.org/wiki/Julian_day
    * http://en.wikipedia.org/wiki/UTC
    * http://en.wikipedia.org/wiki/ISO_8601
*/
class PT_API DateTime {
public:
	enum Months
		/// Symbolic names for month numbers (1 to 12).
	{
		JANUARY = 1,
		FEBRUARY,
		MARCH,
		APRIL,
		MAY,
		JUNE,
		JULY,
		AUGUST,
		SEPTEMBER,
		OCTOBER,
		NOVEMBER,
		DECEMBER
	};
	
	enum DaysOfWeek
		/// Symbolic names for week day numbers (0 to 6).
	{
		SUNDAY = 0,
		MONDAY,
		TUESDAY,
		WEDNESDAY,
		THURSDAY,
		FRIDAY,
		SATURDAY
	};
		
	DateTime();
		/// Creates a DateTime for the current date and time.

	DateTime(const Timestamp& timestamp);
		/// Creates a DateTime for the date and time given in
		/// a Timestamp.
		
	DateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
		/// Creates a DateTime for the given Gregorian date and time.
		///   * year is from 0 to 9999.
		///   * month is from 1 to 12.
		///   * day is from 1 to 31.
		///   * hour is from 0 to 23.
		///   * minute is from 0 to 59.
		///   * second is from 0 to 59.
		///   * millisecond is from 0 to 999.
		///   * microsecond is from 0 to 999.

	DateTime(double julianDay);
		/// Creates a DateTime for the given Julian day.

	DateTime(Timestamp::UtcTimeVal utcTime, Timestamp::TimeDiff diff);
		/// Creates a DateTime from an UtcTimeVal and a TimeDiff.
		///
		/// Mainly used internally by DateTime and friends.

	DateTime(const DateTime& dateTime);
		/// Copy constructor. Creates the DateTime from another one.

	~DateTime();
		/// Destroys the DateTime.

	DateTime& operator = (const DateTime& dateTime);
		/// Assigns another DateTime.
		
	DateTime& operator = (const Timestamp& timestamp);
		/// Assigns a Timestamp.

	DateTime& operator = (double julianDay);
		/// Assigns a Julian day.

	DateTime& assign(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microseconds = 0);
		/// Assigns a Gregorian date and time.
		///   * year is from 0 to 9999.
		///   * month is from 1 to 12.
		///   * day is from 1 to 31.
		///   * hour is from 0 to 23.
		///   * minute is from 0 to 59.
		///   * second is from 0 to 59.
		///   * millisecond is from 0 to 999.
		///   * microsecond is from 0 to 999.

	void swap(DateTime& dateTime);
		/// Swaps the DateTime with another one.

	int year() const;
		/// Returns the year.
		
	int month() const;
		/// Returns the month (1 to 12).
	
	int week(int firstDayOfWeek = MONDAY) const;
		/// Returns the week number within the year.
		/// FirstDayOfWeek should be either SUNDAY (0) or MONDAY (1).
		/// The returned week number will be from 0 to 53. Week number 1 is the week 
		/// containing January 4. This is in accordance to ISO 8601.
		/// 
		/// The following example assumes that firstDayOfWeek is MONDAY. For 2005, which started
		/// on a Saturday, week 1 will be the week starting on Monday, January 3.
		/// January 1 and 2 will fall within week 0 (or the last week of the previous year).
		///
		/// For 2007, which starts on a Monday, week 1 will be the week startung on Monday, January 1.
		/// There will be no week 0 in 2007.
	
	int day() const;
		/// Returns the day witin the month (1 to 31).
		
	int dayOfWeek() const;
		/// Returns the weekday (0 to 6, where
		/// 0 = Sunday, 1 = Monday, ..., 6 = Saturday).
	
	int dayOfYear() const;
		/// Returns the number of the day in the year.
		/// January 1 is 1, February 1 is 32, etc.
	
	int hour() const;
		/// Returns the hour (0 to 23).
		
	int hourAMPM() const;
		/// Returns the hour (0 to 12).
	
	bool isAM() const;
		/// Returns true if hour < 12;

	bool isPM() const;
		/// Returns true if hour >= 12.
		
	int minute() const;
		/// Returns the minute (0 to 59).
		
	int second() const;
		/// Returns the second (0 to 59).
		
	int millisecond() const;
		/// Returns the millisecond (0 to 999)
	
	int microsecond() const;
		/// Returns the microsecond (0 to 999)
	
	double julianDay() const;
		/// Returns the julian day for the date and time.
		
	Timestamp timestamp() const;
		/// Returns the date and time expressed as a Timestamp.

	Timestamp::UtcTimeVal utcTime() const;
		/// Returns the date and time expressed in UTC-based
		/// time. UTC base time is midnight, October 15, 1582.
		/// Resolution is 100 nanoseconds.
		
	bool operator == (const DateTime& dateTime) const;	
	bool operator != (const DateTime& dateTime) const;	
	bool operator <  (const DateTime& dateTime) const;	
	bool operator <= (const DateTime& dateTime) const;	
	bool operator >  (const DateTime& dateTime) const;	
	bool operator >= (const DateTime& dateTime) const;	

	DateTime  operator +  (const Timespan& span) const;
	DateTime  operator -  (const Timespan& span) const;
	Timespan  operator -  (const DateTime& dateTime) const;
	DateTime& operator += (const Timespan& span);
	DateTime& operator -= (const Timespan& span);
	
	static bool isLeapYear(int year);
		/// Returns true if the given year is a leap year;
		/// false otherwise.
		
	static int daysOfMonth(int year, int month);
		/// Returns the number of days in the given month
		/// and year. Month is from 1 to 12.
		
	static bool isValid(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
		/// Checks if the given date and time is valid
		/// (all arguments are within a proper range).
		///
		/// Returns true if all arguments are valid, false otherwise.

	static DateTime fromIsoString(const std::string& s);

    std::string toIsoString() const;

protected:	
	static double toJulianDay(Timestamp::UtcTimeVal utcTime);
		/// Computes the Julian day for an UTC time.
	
	static double toJulianDay(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
		/// Computes the Julian day for a gregorian calendar date and time.
		/// See <http://vsg.cape.com/~pbaum/date/jdimp.htm>, section 2.3.1 for the algorithm.
	
	static Timestamp::UtcTimeVal toUtcTime(double julianDay);
		/// Computes the UTC time for a Julian day.
		
	void computeGregorian(double julianDay);
		/// Computes the Gregorian date for the given Julian day.
		/// See <http://vsg.cape.com/~pbaum/date/injdimp.htm>, section 3.3.1 for the algorithm.

	void computeDaytime();
		/// Extracts the daytime (hours, minutes, seconds, etc.) from the stored utcTime.

private:
	void checkLimit(short& lower, short& higher, short limit);
	void normalize();
		///utility functions used to correct the overflow in computeGregorian

	Timestamp::UtcTimeVal _utcTime;
	short  _year;
	short  _month;
	short  _day;
	short  _hour;
	short  _minute;
	short  _second;
	short  _millisecond;
	short  _microsecond;
};


//
// inlines
//
inline Timestamp DateTime::timestamp() const
{
	return Timestamp::fromUtcTime(_utcTime);
}


inline Timestamp::UtcTimeVal DateTime::utcTime() const
{
	return _utcTime;
}


inline int DateTime::year() const
{
	return _year;
}

	
inline int DateTime::month() const
{
	return _month;
}

	
inline int DateTime::day() const
{
	return _day;
}

	
inline int DateTime::hour() const
{
	return _hour;
}


inline int DateTime::hourAMPM() const
{
	if (_hour < 1)
		return 12;
	else if (_hour > 12)
		return _hour - 12;
	else
		return _hour;
}


inline bool DateTime::isAM() const
{
	return _hour < 12;
}


inline bool DateTime::isPM() const
{
	return _hour >= 12;
}

	
inline int DateTime::minute() const
{
	return _minute;
}

	
inline int DateTime::second() const
{
	return _second;
}

	
inline int DateTime::millisecond() const
{
	return _millisecond;
}


inline int DateTime::microsecond() const
{
	return _microsecond;
}


inline bool DateTime::operator == (const DateTime& dateTime) const
{
	return _utcTime == dateTime._utcTime;
}


inline bool DateTime::operator != (const DateTime& dateTime) const	
{
	return _utcTime != dateTime._utcTime;
}


inline bool DateTime::operator <  (const DateTime& dateTime) const	
{
	return _utcTime < dateTime._utcTime;
}


inline bool DateTime::operator <= (const DateTime& dateTime) const
{
	return _utcTime <= dateTime._utcTime;
}


inline bool DateTime::operator >  (const DateTime& dateTime) const
{
	return _utcTime > dateTime._utcTime;
}


inline bool DateTime::operator >= (const DateTime& dateTime) const	
{
	return _utcTime >= dateTime._utcTime;
}


inline bool DateTime::isLeapYear(int year)
{
	return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
}


inline void swap(DateTime& d1, DateTime& d2)
{
	d1.swap(d2);
}




/*
    class PT_API DateTime
    {
        private:
            Date _date;
            Time _time;
            
        public:
    

        DateTime()
            :    _date()
            ,    _time()
        {}
    

        DateTime(const Date& date, const Time& time)
            :    _date(date)
            ,    _time(time)
        {}
    

        unsigned short hours() const     
        { return _time.hours(); }


        unsigned short minutes() const   
        { return _time.minutes(); }


        unsigned short seconds() const   
        { return _time.seconds(); }


        unsigned short msecs() const   
        { return _time.msecs(); }
        

        unsigned short years() const     
        { return _date.years(); }
        

        unsigned short months() const    
        { return _date.months(); }
        

        unsigned short days() const      
        { return _date.days(); }
        

        bool isNull() const  
        { return _date.isNull(); }


        void set(unsigned short year,
            unsigned short month,
            unsigned short day,
            unsigned short hour,
            unsigned short minute,
            unsigned short second,
            unsigned short msec = 0)
        {
            _date.set(year, month, day);
            _time.set(hour, minute, second, msec);
        }
                

        std::string toIsoString() const;
        

        static DateTime fromIsoString(const std::string& s);
        

        bool operator== (const DateTime& dt) const
        { 
            return _date == dt._date
                && _time == dt._time;
        }


        bool operator!= (const DateTime& dt) const
        { return !operator==(dt); }


        bool operator< (const DateTime& dt) const
        { 
            return this->years() < dt.years()
                    || this->years() == dt.years()
                    && ( this->months() < dt.months()
                    || this->months() == dt.months()
                        && ( this->days() < dt.days()
                        || this->days() == dt.days()
                        && ( this->hours() < dt.hours()
                            || this->hours() == dt.hours()
                            && ( this->minutes() < dt.minutes()
                                || this->minutes() == dt.minutes()
                                && ( this->seconds() < dt.seconds()
                                || this->seconds() == dt.seconds()
                                    && this->msecs() < dt.msecs()
                                )
                            )
                        )
                    )
            ); 
        }


        bool operator> (const DateTime& dt) const
        { return dt < *this; }


        bool operator<= (const DateTime& dt) const
        { return !(*this > dt); }


        bool operator>= (const DateTime& dt) const
        { return !(*this < dt); }
        
        const Date& date()
        { return _date; }
        
        const Time& time()
        { return _time; }
    };
*/
}

#endif // PT_DATETIME_H
