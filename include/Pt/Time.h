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
#ifndef PT_TIME_H
#define PT_TIME_H

#include <Pt/Api.h>
#include <Pt/Timespan.h>
#include <Pt/SerializationInfo.h>
#include <string>


namespace Pt {

class InvalidTime : public std::invalid_argument
{
    public:
        InvalidTime(const SourceInfo& si);

        ~InvalidTime() throw();
};


/** @brief %Time expressed in hours, minutes, seconds and milliseconds
    @ingroup DateTime
*/
class PT_API Time
{
    public:
        static const int SecsPerDay = 86400;
        static const int MSecsPerDay = 86400000;
        static const int SecsPerHour = 3600;
        static const int MSecsPerHour = 3600000;
        static const int SecsPerMin = 60;
        static const int MSecsPerMin = 60000;

        /** \brief Creates a Time set to zero.
        */
        Time()
        : _msecs(0)
        {}

        /** \brief Creates a Time from given values.

            InvalidTime is thrown if one or more of the values are out of range
        */
        Time(unsigned hour, unsigned min, unsigned ses = 0, unsigned msec = 0);

        /** \brief Returns the hour-part of the Time.
        */
        unsigned hour() const;

        /** \brief Returns the minute-part of the Time.
        */
        unsigned minute() const;

        /** \brief Returns the second-part of the Time.
        */
        unsigned second() const;

        /** \brief Returns the millisecond-part of the Time.
        */
        unsigned msec() const;

        unsigned totalMSecs() const
        { return _msecs; }

        void setTotalMSecs(unsigned msecs)
        { _msecs = msecs; }

        /** \brief Sets the time.

            Sets the time to a new hour, minute, second, milli-second.
            InvalidTime is thrown if one or more of the values are out of range
        */
        void set(unsigned hour, unsigned min, unsigned sec, unsigned mssec = 0);

        /** @brief Get the time values

            Gets the hour, minute, second and millisecond parts of the time.
        */
        void get(unsigned& h, unsigned& m, unsigned& s, unsigned& ms) const;

        /** @brief Adds seconds to the time

            This method does not change the time, but returns the time
            with the seconds added.
        */
        Time addSecs(int secs) const;

        /** @brief Determines seconds until another time
        */
        int secsTo(const Time &) const;

        /** @brief Adds milliseconds to the time

            This method does not change the time, but returns the time
            with the milliseconds added.
        */
        Time addMSecs(Pt::int64_t ms) const;

        /** @brief Determines milliseconds until another time
        */
        int msecsTo(const Time &) const;

        /** @brief Assignment operator
        */
        Time& operator=(const Time& other)
        { _msecs=other._msecs; return *this; }

        /** @brief Equal comparison operator
        */
        bool operator==(const Time& other) const
        { return _msecs == other._msecs; }

        /** @brief Inequal comparison operator
        */
        bool operator!=(const Time& other) const
        { return _msecs != other._msecs; }

        /** @brief Less-than comparison operator
        */
        bool operator<(const Time& other) const
        { return _msecs < other._msecs; }

        /** @brief Less-than-or-equal comparison operator
        */
        bool operator<=(const Time& other) const
        { return _msecs <= other._msecs; }

        /** @brief Greater-than comparison operator
        */
        bool operator>(const Time& other) const
        { return _msecs > other._msecs; }

        /** @brief Greater-than-or-equal comparison operator
        */
        bool operator>=(const Time& other) const
        { return _msecs >= other._msecs; }

        /** @brief Assignment by sum operator
        */
        Time& operator+=(const Timespan& ts)
        {
            Pt::int64_t msecs = ( _msecs + ts.totalMSecs() ) % MSecsPerDay;
            _msecs = static_cast<unsigned>(msecs);
            return *this;
        }

        /** @brief Assignment by difference operator
        */
        Time& operator-=(const Timespan& ts)
        {
            Pt::int64_t msecs = ts.totalMSecs();
            Pt::int64_t days = ( MSecsPerDay - msecs ) / MSecsPerDay;
            msecs = (_msecs + msecs + days * MSecsPerDay) % MSecsPerDay;
            _msecs = static_cast<unsigned>(msecs);
            return *this;
        }

        /** @brief Addition operator
        */
        friend Time operator+(const Time& time, const Timespan& ts)
        {
            return time.addMSecs( ts.totalMSecs() );
        }

        /** @brief Substraction operator
        */
        friend Time operator-(const Time& time, const Timespan& ts)
        {
            return time.addMSecs( -ts.totalMSecs() );
        }

        /** @brief Substraction operator
        */
        friend Timespan operator-(const Time& a, const Time& b)
        {

            return b.msecsTo(a);
        }

        /** \brief Returns the time in ISO-format (hh:mm:ss.hhh)
        */
        std::string toIsoString() const;

    public:
        // TODO: move to Pt::System
        //static Time currentTime();

        /** \brief Returns true if values are a valid time
        */
        static bool isValid(unsigned h, unsigned m, unsigned s, unsigned ms = 0);

        /** \brief Convert from an ISO time string

            Interprets the passed string as a time-string in ISO-format
            (hh:mm:ss.hhh) and returns a Time-object. If the string is not
            in ISO-format, InvalidTime is thrown.
        */
        static Time fromIsoString(const std::string& s);

    private:
        //! @internal
        unsigned _msecs;
    };


    inline void operator >>=(const SerializationInfo& si, Time& time)
    {
        unsigned hour = si.getValue<unsigned>("hour");
        unsigned min = si.getValue<unsigned>("minute");
        unsigned sec = si.getValue<unsigned>("second");
        unsigned msec = si.getValue<unsigned>("millisec");
        time.set(hour, min, sec, msec);
    }


    inline void operator <<=(SerializationInfo& si, const Time& time)
    {
        unsigned hour = 0;
        unsigned min = 0;
        unsigned sec = 0;
        unsigned msec = 0;
        time.get(hour, min, sec, msec);

        si.addValue("hour", hour );
        si.addValue("minute", min );
        si.addValue("second", sec );
        si.addValue("millisec", msec );
        si.setTypeName("Time");
    }

}

#endif // PT_TIME_H
