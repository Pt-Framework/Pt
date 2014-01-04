/*
 * Copyright (C) 2006-2013 by Marc Boris Duerner
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

#ifndef Pt_Timespan_h
#define Pt_Timespan_h

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <limits>

namespace Pt {

/** @brief  Represents time spans up to microsecond resolution.

    @ingroup DateTime
*/
class Timespan
{
    private:
        static const Pt::int64_t Milliseconds = 1000;
        static const Pt::int64_t Seconds      = 1000 * Timespan::Milliseconds;
        static const Pt::int64_t Minutes      =   60 * Timespan::Seconds;
        static const Pt::int64_t Hours        =   60 * Timespan::Minutes;
        static const Pt::int64_t Days         =   24 * Timespan::Hours;

    public:
        //! @brief Creates a zero Timespan.
        Timespan()
        : _span(0)
        {}

        //! @brief Creates a Timespan.
        explicit Timespan(Pt::int64_t microseconds)
        : _span(microseconds)
        { }

        /** @brief Creates a Timespan.
            Useful for creating a Timespan from a POSIX timeval struct.
        */
        Timespan(long secs, long microsecs)
        : _span(Pt::int64_t(secs)*Seconds + microsecs)
        { }

        //! @brief Creates a Timespan.
        Timespan(int days, int hours, int minutes, int secs, int microseconds);

        //! @brief Creates a Timespan from another one.
        Timespan(const Timespan& timespan);

        //! @brief Destroys the Timespan.
        ~Timespan()
        {}

        //! @brief Assignment operator.
        Timespan& operator=(const Timespan& timespan);

        //! @brief Assigns a new span.
        Timespan& set(int days, int hours, int minutes, int seconds, int microseconds);

        /** @brief Assigns a new span.
            Useful for assigning from a POSIX timeval struct.
        */
        Timespan& set(long seconds, long microseconds);

        bool isNull() const;

        void setNull();

        Timespan& operator+=(const Timespan& d);

        Timespan& operator-=(const Timespan& d);

        //! @brief Returns the number of days.
        int days() const;

        //! @brief Returns the number of hours (0 to 23).
        int hours() const;

        //! @brief Returns the total number of hours.
        int toHours() const;

        //! @brief Returns the number of minutes (0 to 59).
        int minutes() const;

        //! @brief Returns the total number of minutes.
        int toMinutes() const;

        //! @brief Returns the number of seconds (0 to 59).
        int seconds() const;

        //! @brief Returns the total number of seconds.
        int toSeconds() const;

        //! @brief Returns the number of milliseconds (0 to 999).
        int msecs() const;

        //! @brief Returns the total number of milliseconds.
        Pt::int64_t toMSecs() const;

        //! @brief Returns the fractions of a millisecond in microseconds (0 to 999).
        int usecs() const
        { return int(_span % 1000); }

        //! @brief Returns the total number of microseconds.
        inline Pt::int64_t toUSecs() const
        { return _span; }

        // @internal
        inline static Pt::int64_t maxMSecs()
        { return std::numeric_limits<Pt::int64_t>::max() / 1000; }

    private:
        Pt::int64_t _span;
};


inline bool operator ==(const Timespan& a, const Timespan& b)
{ return a.toUSecs() == b.toUSecs(); }


inline bool operator !=(const Timespan& a, const Timespan& b)
{ return a.toUSecs() != b.toUSecs(); }


inline bool operator >(const Timespan& a, const Timespan& b)
{ return a.toUSecs() > b.toUSecs(); }


inline bool operator >=(const Timespan& a, const Timespan& b)
{ return a.toUSecs() >= b.toUSecs(); }


inline bool operator <(const Timespan& a, const Timespan& b)
{ return a.toUSecs() < b.toUSecs(); }


inline bool operator <=(const Timespan& a, const Timespan& b)
{ return a.toUSecs() <= b.toUSecs(); }


inline Timespan operator +(const Timespan& a, const Timespan& b)
{ return Timespan(a.toUSecs() + b.toUSecs()); }


inline Timespan operator -(const Timespan& a, const Timespan& b)
{ return Timespan(a.toUSecs() - b.toUSecs()); }


inline int Timespan::days() const
{
    return int(_span/Days);
}


inline int Timespan::hours() const
{
    return int((_span/Hours) % 24);
}


inline int Timespan::toHours() const
{
    return int(_span/Hours);
}


inline int Timespan::minutes() const
{
    return int((_span/Minutes) % 60);
}


inline int Timespan::toMinutes() const
{
    return int(_span/Minutes);
}


inline int Timespan::seconds() const
{
    return int((_span/Seconds) % 60);
}


inline int Timespan::toSeconds() const
{
    return int(_span/Seconds);
}


inline int Timespan::msecs() const
{
    return int((_span/Milliseconds) % 1000);
}


inline Pt::int64_t Timespan::toMSecs() const
{
    return _span/Milliseconds;
}


inline Timespan::Timespan(int d, int h, int mins, int secs, int microsecs)
: _span( Pt::int64_t(microsecs) +
         Pt::int64_t(secs)*Seconds +
         Pt::int64_t(mins)*Minutes +
         Pt::int64_t(h)*Hours +
         Pt::int64_t(d)*Days )
{
}


inline Timespan::Timespan(const Timespan& timespan)
: _span(timespan._span)
{
}


inline Timespan& Timespan::operator=(const Timespan& timespan)
{
    _span = timespan._span;
    return *this;
}


inline Timespan& Timespan::set(int d, int h, int mins, int secs, int microsecs)
{
    _span = Pt::int64_t(microsecs) +
            Pt::int64_t(secs)*Seconds +
            Pt::int64_t(mins)*Minutes +
            Pt::int64_t(h)*Hours +
            Pt::int64_t(d)*Days;
    return *this;
}


inline Timespan& Timespan::set(long secs, long microseconds)
{
    _span = Pt::int64_t(secs)*Seconds + Pt::int64_t(microseconds);
    return *this;
}


inline bool Timespan::isNull() const
{
    return _span == 0;
}


inline void Timespan::setNull()
{
    _span = 0;
}

inline Timespan& Timespan::operator += (const Timespan& d)
{
    _span += d._span;
    return *this;
}


inline Timespan& Timespan::operator -= (const Timespan& d)
{
    _span -= d._span;
    return *this;
}

} // namespace Pt

#endif // Pt_Timespan_h
