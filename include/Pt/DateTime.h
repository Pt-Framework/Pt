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
PT_API std::string dateTimeToString(const DateTime& dt, int* utcOffset);

//! @internal
PT_API DateTime dateTimeFromString(const std::string& s);

//! @internal
PT_API DateTime dateTimeFromString(const std::string& s, int* utcOffset);

/** @brief Combined %Date and %Time value.

    %Pt::DateTime combines a Pt::Date and a Pt::Time object into one instance.
    It can either be constructed from the corrsponding numeric values or a 
    string in ISO format. The date and time parts can be accessed with date()
    and time(). When two DateTimes are compared, one is considered less, if
    its date is earlier or if the time is earlier in case of equal dates.
    A %Pt::Timespan can be added or subtracted from a %DateTime and this is
    also the result when two DateTimes are subtracted. To avoid the exceptions
    thrown by the underlying time and date, isValid() can be used
    to check numeric date and time values.

    @ingroup BasicTypes
*/
class DateTime
{
    public:
        /** @brief Default Constructor.
        */
        DateTime()
        { }

        /** @brief Construct to date and time.
        */
        DateTime(int y, unsigned mon, unsigned d,
                 unsigned h = 0, unsigned min = 0, 
                 unsigned s = 0, unsigned ms = 0)
        : _date(y, mon, d)
        , _time(h, min, s, ms)
        { }

        /** @brief Copy Constructor.
        */
        DateTime(const DateTime& dateTime)
        : _date( dateTime.date() )
        , _time( dateTime.time() )
        { }

        /** @brief Assignment operator.
        */
        DateTime& operator=(const DateTime& dateTime);

        /** @brief Sets the date and time.
        */
        void set(int year, unsigned month, unsigned day,
                 unsigned hour = 0, unsigned min = 0, unsigned sec = 0, unsigned msec = 0);

        /** @brief Gets the date and time.
        */
        void get(int& year, unsigned& month, unsigned& day,
                 unsigned& hour, unsigned& min, unsigned& sec, unsigned& msec) const;

        /** @brief Gets the date.
        */
        const Date& date() const
        { return _date; }

        /** @brief Gets the date.
        */
        Date& date()
        { return _date; }

        /** @brief Sets the date.
        */
        DateTime& setDate(const Date& dt)
        { _date = dt; return *this; }

        /** @brief Gets the time.
        */
        const Time& time() const
        { return _time; }

        /** @brief Gets the time.
        */
        Time& time()
        { return _time; }

        /** @brief Sets the time.
        */
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

        /** \brief Returns the date and time in ISO-format
        */
        std::string toIsoString() const
        { return dateTimeToString(*this); }

        /** \brief Returns the date and time in ISO-format
        */
        std::string toIsoString(int* utcOffset) const
        { return dateTimeToString(*this, utcOffset); }

        /** \brief Interprets a string as a date and time in ISO-format
        */
        static DateTime fromIsoString(const std::string& s)
        { return dateTimeFromString(s); }

        /** \brief Interprets a string as a date and time in ISO-format
        */
        static DateTime fromIsoString(const std::string& s, int* utcOffset)
        { return dateTimeFromString(s, utcOffset); }

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

        //! @brief Returns true if values are a valid date and time 
        static bool isValid(int year, unsigned month, unsigned day,
                            unsigned hour, unsigned minute, unsigned second, unsigned msec);

    private:
        //! @internal
        DateTime(unsigned jd)
        : _date(jd)
        {}

    private:
        Date _date;
        Time _time;
};


/** @brief Deserialize a %DateTime.

    @related DateTime
*/
PT_API void operator >>=(const SerializationInfo& si, DateTime& dt);

/** @brief Serialize a %DateTime.

    @related DateTime
*/
PT_API void operator <<=(SerializationInfo& si, const DateTime& dt);

/** @brief Add a timespan.

    @related DateTime
*/
inline DateTime operator+(const DateTime& dt, const Timespan& ts)
{
    DateTime tmp = dt;
    tmp += ts;
    return tmp;
}

/** @brief Subtract two DateTimes.

    @related DateTime
*/
inline Timespan operator-(const DateTime& first, const DateTime& second)
{
    Pt::int64_t dayDiff      = Pt::int64_t( first.date().julian() ) -
                                Pt::int64_t( second.date().julian() );

    Pt::int64_t milliSecDiff = Pt::int64_t( first.time().toMSecs() ) -
                                Pt::int64_t( second.time().toMSecs() );

    Pt::int64_t result = (dayDiff * Time::MSecsPerDay + milliSecDiff) * 1000;

    return Timespan(result);
}

/** @brief Subtract a timespan.

    @related DateTime
*/
inline DateTime operator-(const DateTime& dt, const Timespan& ts)
{
    DateTime tmp = dt;
    tmp -= ts;
    return tmp;
}

/** @brief Less-than comparison operator.

    @related DateTime
*/
inline bool operator< (const DateTime& a, const DateTime& b)
{
    return a.date() < b.date()
        || (a.date() == b.date()
          && a.time() < b.time());
}

/** @brief Less-than-equal comparison operator.

    @related DateTime
*/
inline bool operator<= (const DateTime& a, const DateTime& b)
{
    return a.date() < b.date()
        || (a.date() == b.date()
          && a.time() <= b.time());
}

/** @brief Greater-than comparison operator.

    @related DateTime
*/
inline bool operator> (const DateTime& a, const DateTime& b)
{
    return a.date() > b.date()
        || (a.date() == b.date()
          && a.time() > b.time());
}

/** @brief Greater-than-equal comparison operator.

    @related DateTime
*/
inline bool operator>= (const DateTime& a, const DateTime& b)
{
    return a.date() > b.date()
        || (a.date() == b.date()
          && a.time() >= b.time());
}

/** @brief Returns true if equal.

    @related DateTime
*/
inline bool operator==(const DateTime& a, const DateTime& b)
{
    return a.date() == b.date() && a.time() == b.time();
}

/** @brief Returns true if not equal.

    @related DateTime
*/
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
