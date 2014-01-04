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

#ifndef PT_DATETIME_H
#define PT_DATETIME_H

#include <Pt/Api.h>
#include <Pt/Time.h>
#include <Pt/Date.h>
#include <string>

namespace Pt {

//! @internal
PT_API std::string dateTimeToString(const DateTime& dt);

//! @internal
PT_API DateTime dateTimeFromString(const std::string& s);

/** @brief Combined %Date and %Time value.

    @ingroup DateTime
*/
class DateTime
{
    public:
        DateTime()
        { }

        DateTime(int y, unsigned mon, unsigned d,
                 unsigned h = 0, unsigned min = 0, 
                 unsigned s = 0, unsigned ms = 0)
        : _date(y, mon, d)
        , _time(h, min, s, ms)
        { }

        DateTime(const DateTime& dateTime)
        : _date( dateTime.date() )
        , _time( dateTime.time() )
        { }

        DateTime& operator=(const DateTime& dateTime);

        void set(int year, unsigned month, unsigned day,
                 unsigned hour = 0, unsigned min = 0, unsigned sec = 0, unsigned msec = 0);

        void get(int& year, unsigned& month, unsigned& day,
                 unsigned& hour, unsigned& min, unsigned& sec, unsigned& msec) const;

        const Date& date() const
        { return _date; }

        Date& date()
        { return _date; }

        DateTime& setDate(const Date& dt)
        { _date = dt; return *this; }

        const Time& time() const
        { return _time; }

        Time& time()
        { return _time; }

        DateTime& setTime(const Time& t)
        { _time = t; return *this; }

        /** @brief Returns the day-part of the date.
        */
        unsigned day() const
        { return date().day(); }

        /** @brief Returns the month-part of the date.
        */
        unsigned month() const
        { return date().month(); }

        /** @brief Returns the year-part of the date.
        */
        int year() const
        { return date().year(); }

        /** \brief Returns the hour-part of the Time.
        */
        unsigned hour() const
        { return time().hour(); }

        /** \brief Returns the minute-part of the Time.
        */
        unsigned minute() const
        { return time().minute(); }

        /** \brief Returns the second-part of the Time.
        */
        unsigned second() const
        { return time().second(); }

        /** \brief Returns the millisecond-part of the Time.
        */
        unsigned msec() const
        { return time().msec(); }

        std::string toIsoString() const
        { return dateTimeToString(*this); }

        static DateTime fromIsoString(const std::string& s)
        { return dateTimeFromString(s); }

        /** @brief Assignment by sum operator
        */
        DateTime& operator+=(const Timespan& ts)
        {
            Pt::int64_t totalMSecs = ts.toMSecs();
            Pt::int64_t days = totalMSecs / Time::MSecsPerDay;
            Pt::int64_t overrun = totalMSecs % Time::MSecsPerDay;

            if( (-overrun) > _time.toMSecs()  )
            {
                days -= 1;
            }
            else if( overrun + _time.toMSecs() > Time::MSecsPerDay)
            {
                days += 1;
            }

            _date += static_cast<int>(days);
            _time += Timespan(overrun * 1000);
            return *this;
        }

        /** @brief Assignment by difference operator
        */
        DateTime& operator-=(const Timespan& ts)
        {
            Pt::int64_t totalMSecs = ts.toMSecs();
            Pt::int64_t days = totalMSecs / Time::MSecsPerDay;
            Pt::int64_t overrun = totalMSecs % Time::MSecsPerDay;

            if( overrun > _time.toMSecs() )
            {
                days += 1;
            }
            else if(_time.toMSecs() - overrun > Time::MSecsPerDay)
            {
                days -= 1;
            }

            _date -= static_cast<int>(days);
            _time -= Timespan( overrun * 1000 );
            return *this;
        }

        static bool isValid(int year, unsigned month, unsigned day,
                            unsigned hour, unsigned minute, unsigned second, unsigned msec);

    private:
        DateTime(unsigned jd)
        : _date(jd)
        {}

    private:
        Date _date;
        Time _time;
};


PT_API void operator >>=(const SerializationInfo& si, DateTime& dt);


PT_API void operator <<=(SerializationInfo& si, const DateTime& dt);


inline DateTime operator+(const DateTime& dt, const Timespan& ts)
{
    DateTime tmp = dt;
    tmp += ts;
    return tmp;
}


inline Timespan operator-(const DateTime& first, const DateTime& second)
{
    Pt::int64_t dayDiff      = Pt::int64_t( first.date().julian() ) -
                                Pt::int64_t( second.date().julian() );

    Pt::int64_t milliSecDiff = Pt::int64_t( first.time().toMSecs() ) -
                                Pt::int64_t( second.time().toMSecs() );

    Pt::int64_t result = (dayDiff * Time::MSecsPerDay + milliSecDiff) * 1000;

    return Timespan(result);
}


inline DateTime operator-(const DateTime& dt, const Timespan& ts)
{
    DateTime tmp = dt;
    tmp -= ts;
    return tmp;
}


inline bool operator< (const DateTime& a, const DateTime& b)
{
    return a.date() < b.date()
        || (a.date() == b.date()
          && a.time() < b.time());
}

inline bool operator<= (const DateTime& a, const DateTime& b)
{
    return a.date() < b.date()
        || (a.date() == b.date()
          && a.time() <= b.time());
}

inline bool operator> (const DateTime& a, const DateTime& b)
{
    return a.date() > b.date()
        || (a.date() == b.date()
          && a.time() > b.time());
}

inline bool operator>= (const DateTime& a, const DateTime& b)
{
    return a.date() > b.date()
        || (a.date() == b.date()
          && a.time() >= b.time());
}


inline bool operator==(const DateTime& a, const DateTime& b)
{
    return a.date() == b.date() && a.time() == b.time();
}

inline bool operator!=(const DateTime& a, const DateTime& b)
{
    return a.date() != b.date() || a.time() != b.time();
}


inline DateTime& DateTime::operator=(const DateTime& dateTime)
{
	_date = dateTime.date();
	_time = dateTime.time();
	return *this;
}


inline void DateTime::set(int y, unsigned mon, unsigned d,
                          unsigned h, unsigned min, unsigned s, unsigned ms)
{
    _date.set(y, mon, d);
    _time.set(h, min, s, ms);
}


inline void DateTime::get(int& y, unsigned& mon, unsigned& d,
                          unsigned& h, unsigned& min, unsigned& s, unsigned& ms) const
{
    _date.get(y, mon, d);
    _time.get(h, min, s, ms);
}


inline bool DateTime::isValid(int year, unsigned month, unsigned day,
                              unsigned hour, unsigned minute, unsigned second, unsigned msec)
{
    return Date::isValid(year, month, day) && Time::isValid(hour, minute, second, msec);
}

} // namespace Pt

#endif // PT_DATETIME_H
