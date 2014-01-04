/*
 * Copyright (C) 2006-2008 by Marc Boris Duerner
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

#ifndef PT_TIME_H
#define PT_TIME_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/String.h>
#include <Pt/Timespan.h>
#include <string>
#include <stdexcept>

namespace Pt {

class SerializationInfo;

/** @brief Indicates an invalid time value.

    This exception is thrown, when a invalid values are used for a time,
    such as a value greater than 23 for the hour. 

    @ingroup DateTime
*/
class PT_API InvalidTime : public std::runtime_error
{
    public:
        //! @brief Constructor.
        InvalidTime();

        //! @brief Destructor.
        ~InvalidTime() throw()
        {}
};

//! @internal
PT_API std::string timeToString(const Time& t);

//! @internal
PT_API Time timeFromString(const std::string& s);

/** @brief %Time expressed in hours, minutes, seconds and milliseconds
    @ingroup DateTime
*/
class Time
{
    public:
        static const uint32_t MaxHours         = 23;
        static const uint32_t HoursPerDay      = 24;
        static const uint32_t MaxMinutes       = 59;
        static const uint32_t MinutesPerHour   = 60;
        static const uint32_t MinutesPerDay    = 1440;
        static const uint32_t MaxSeconds       = 59;
        static const uint32_t SecondsPerDay    = 86400;
        static const uint32_t SecondsPerHour   = 3600;
        static const uint32_t SecondsPerMinute = 60;
        static const uint32_t MSecsPerDay      = 86400000;
        static const uint32_t MSecsPerHour     = 3600000;
        static const uint32_t MSecsPerMinute   = 60000;
        static const uint32_t MSecsPerSecond   = 1000;

    public:
        /** \brief Creates a Time set to zero.
        */
        Time()
        : _msecs(0)
        {}

        /** \brief Creates a Time from given values.

            InvalidTime is thrown if one or more of the values are out of range
        */
        inline Time(unsigned h, unsigned m, unsigned s = 0, unsigned ms = 0)
        : _msecs(0)
        {
            set(h, m, s, ms);
        }

        /** @brief Assignment operator
        */
        Time& operator=(const Time& other)
        { _msecs=other._msecs; return *this; }

        /** \brief Returns the hour-part of the Time.
        */
        unsigned hour() const
        {
            return _msecs / MSecsPerHour;
        }

        /** \brief Returns the minute-part of the Time.
        */
        unsigned minute() const
        {
            return (_msecs % MSecsPerHour) / MSecsPerMinute;
        }

        /** \brief Returns the second-part of the Time.
        */
        unsigned second() const
        {
            return (_msecs / 1000) % SecondsPerMinute;
        }

        /** \brief Returns the millisecond-part of the Time.
        */
        unsigned msec() const
        {
            return _msecs % 1000;
        }

        inline uint32_t toMSecs() const
        { return _msecs; }

        void setTotalMSecs(uint32_t msecs)
        { _msecs = msecs; }

        /** \brief Sets the time.

            Sets the time to a new hour, minute, second, milli-second.
            InvalidTime is thrown if one or more of the values are out of range
        */
        void set(unsigned h, unsigned m, unsigned s, unsigned ms = 0)
        {
            if ( ! isValid(h, m, s , ms) )
            {
                throw InvalidTime();
            }

            _msecs = (h * SecondsPerHour + m * SecondsPerMinute + s) * 1000 + ms;
        }

        /** @brief Get the time values

            Gets the hour, minute, second and millisecond parts of the time.
        */
        void get(unsigned& h, unsigned& m, unsigned& s, unsigned& ms) const
        {
            h = hour();
            m = minute();
            s = second();
            ms = msec();
        }

        /** @brief Adds seconds to the time

            This method does not change the time, but returns the time
            with the seconds added.
        */
        Time addSecs(int secs) const
        {
            return addMSecs(secs * 1000);
        }

        /** @brief Determines seconds until another time
        */
        int secsUntil(const Time &t) const
        {
            return static_cast<int>( msecsUntil(t) / 1000 );
        }

        /** @brief Adds milliseconds to the time

            This method does not change the time, but returns the time
            with the milliseconds added.
        */
        inline Time addMSecs(Pt::int64_t ms) const
        {
            Time t;
            if (ms < 0)
            {
                Pt::int64_t negdays = (MSecsPerDay - ms) / MSecsPerDay;
                t._msecs = static_cast<uint32_t>((_msecs + ms + negdays * MSecsPerDay) % MSecsPerDay);
            }
            else
            {
                t._msecs = static_cast<uint32_t>((_msecs + ms) % MSecsPerDay);
            }

            return t;
        }

        /** @brief Determines milliseconds until another time
        */
        Pt::int64_t msecsUntil(const Time &t) const
        {
            if(t._msecs > _msecs)
                return t._msecs - _msecs;

            return MSecsPerDay - (_msecs - t._msecs);
        }

        /** \brief Returns the time in ISO-format (hh:mm:ss.hhh)
        */
        std::string toIsoString() const
        { return timeToString(*this); }

        /** \brief Convert from an ISO time string

            Interprets the passed string as a time-string in ISO-format
            (hh:mm:ss.hhh) and returns a Time-object. If the string is not
            in ISO-format, InvalidTime is thrown.
        */
        static Time fromIsoString(const std::string& s)
        { return timeFromString(s); }

        /** @brief Assignment by sum operator
        */
        Time& operator+=(const Timespan& ts)
        {
            Pt::int64_t msecs = ( _msecs + ts.toMSecs() ) % MSecsPerDay;
            msecs = msecs < 0 ? MSecsPerDay + msecs : msecs;
            _msecs = static_cast<uint32_t>(msecs);
            return *this;
        }

        /** @brief Assignment by difference operator
        */
        Time& operator-=(const Timespan& ts)
        {
            Pt::int64_t msecs = ( _msecs - ts.toMSecs() ) % MSecsPerDay;
            msecs = msecs < 0 ? MSecsPerDay + msecs : msecs;
            _msecs = static_cast<uint32_t>(msecs);
            return *this;
        }

        /** \brief Returns true if values are a valid time
        */
        static bool isValid(unsigned h, unsigned m, unsigned s, unsigned ms)
        {
            return h < 24 && m < 60 && s < 60 && ms < 1000;
        }

    private:
        //! @internal
        Pt::uint32_t _msecs;
};


PT_API void operator >>=(const SerializationInfo& si, Time& time);


PT_API void operator <<=(SerializationInfo& si, const Time& time);


/** @brief Equal comparison operator
*/
inline bool operator==(const Time& a, const Time& b)
{ return a.toMSecs() == b.toMSecs(); }

/** @brief Inequal comparison operator
*/
inline bool operator!=(const Time& a, const Time& b)
{ return a.toMSecs() != b.toMSecs(); }

/** @brief Less-than comparison operator
*/
inline bool operator<(const Time& a, const Time& b)
{ return a.toMSecs() < b.toMSecs(); }

/** @brief Less-than-or-equal comparison operator
*/
inline bool operator<=(const Time& a, const Time& b)
{ return a.toMSecs() <= b.toMSecs(); }

/** @brief Greater-than comparison operator
*/
inline bool operator>(const Time& a, const Time& b)
{ return a.toMSecs() > b.toMSecs(); }

/** @brief Greater-than-or-equal comparison operator
*/
inline bool operator>=(const Time& a, const Time& b)
{ return a.toMSecs() >= b.toMSecs(); }

/** @brief Addition operator
*/
inline Time operator+(const Time& time, const Timespan& ts)
{ return time.addMSecs( ts.toMSecs() ); }

/** @brief Substraction operator
*/
inline Time operator-(const Time& time, const Timespan& ts)
{ return time.addMSecs( -ts.toMSecs() ); }

/** @brief Substraction operator
*/
inline Timespan operator-(const Time& a, const Time& b)
{ return Timespan( b.msecsUntil(a) * 1000 ); }

}

#endif // PT_TIME_H
