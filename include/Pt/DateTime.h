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
#include <string>
#include <map>

namespace Pt {

/** @brief Combined %Date and %Time value
    @ingroup DateTime
*/
class PT_API DateTime
{
    public:
        DateTime();

        DateTime(int year, unsigned month, unsigned day,
                 unsigned hour = 0, unsigned min = 0, unsigned sec = 0, unsigned msec = 0);

        //DateTime(unsigned julianDay);

        DateTime(const DateTime& dateTime);
        
        /**
         * @brief Creates a DateTime object with the specified milliseconds since January 1st 1970.
         *
         * The construction does currently not take care of any time zones. I.e. the milliseconds will
         * be treated as if they were in the same time zone as the reference (January 1st 1970).
         * Thus specifying a "time-zoned" millisecond value will lead to a "time-zoned" DateTime. And
         * accordingly a "GMT" millisecond value will lead to a "GMT" DateTime.
         *
         * @param msecsUnixEpoch The elapsed milliseconds since or until January 1st 1970.
         */
        DateTime(const Pt::int64_t msecsUnixEpoch);

        ~DateTime();

        DateTime& operator=(const DateTime& dateTime);

        DateTime& operator=(unsigned julianDay);

        void set(int year, unsigned month, unsigned day,
                 unsigned hour = 0, unsigned min = 0, unsigned sec = 0, unsigned msec = 0);

        void get(int& year, unsigned& month, unsigned& day,
                 unsigned& hour, unsigned& min, unsigned& sec, unsigned& msec) const;

        const Date& date() const
        { return _date; }

        const Date& date()
        { return _date; }

        DateTime& setDate(const Date& date)
        { _date = date; return *this; }

        const Time& time() const
        { return _time; }

        const Time& time()
        { return _time; }

        DateTime& setTime(const Time& time)
        { _time = time; return *this; }

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
        
        /**
         * @brief Returns the milliseconds between January 1st 1970 and this DateTime object.
         *
         * The calculation does currently not take care of any time zones. I.e. the milliseconds will
         * be calculated as if they were in the same time zone as the reference (January 1st 1970).
         * Thus calling this API on a "time-zoned" DateTime will lead to a "time-zoned" millisecond value. And
         * accordingly calling this API on a "GMT" DateTime will lead to a "GMT" millisecond value.
         *
         * @return The milliseconds between January 1st 1970 and this DateTime object.
         */
        Pt::int64_t msecsUnixEpoch() const;

        static DateTime fromIsoString(const std::string& s);

        std::string toIsoString() const;

        static bool isValid(int year, unsigned month, unsigned day,
                            unsigned hour, unsigned minute, unsigned second, unsigned msec);


        bool operator==(const DateTime& rhs) const
        {
            return !operator!=(rhs);
        }

        bool operator!=(const DateTime& rhs) const
        {
            return _date != rhs._date || _time != rhs._time ;
        }

        /** @brief Assignment by sum operator
        */
        DateTime& operator+=(const Timespan& ts)
        {
            if( ts.totalMSecs() < 0 )
            {
                return this->operator -=( Timespan(-ts.totalMSecs()*1000) );
            }
            else
            {
                Pt::int64_t totalMSecs = ts.totalMSecs();
                Pt::int64_t days = totalMSecs / Time::MSECS_PER_DAY;

                Pt::int64_t overrun = totalMSecs % Time::MSECS_PER_DAY;
                if( overrun + _time.totalMSecs() > Time::MSECS_PER_DAY)
                    days += 1;

                _date += static_cast<int>(days);
                _time += Timespan(overrun * 1000);
                return *this;
            }
        }

        /** @brief Assignment by difference operator
        */
        DateTime& operator-=(const Timespan& ts)
        {
            if( ts.totalMSecs() < 0 )
            {
                return this->operator +=( Timespan(-ts.totalMSecs()*1000) );
            }
            else
            {
                Pt::int64_t totalMSecs = ts.totalMSecs();
                Pt::int64_t days = totalMSecs / Time::MSECS_PER_DAY;

                Pt::int64_t overrun = totalMSecs % Time::MSECS_PER_DAY;
                if( overrun > _time.totalMSecs() )
                    days += 1;

                _date -= static_cast<int>(days);
                _time -= Timespan( overrun * 1000 );
                return *this;
            }
        }

        friend Timespan operator-(const DateTime& first, const DateTime& second)
        {
            Pt::int64_t dayDiff      = Pt::int64_t( first.date().julian() ) -
                                       Pt::int64_t( second.date().julian() );

            Pt::int64_t milliSecDiff = Pt::int64_t( first.time().totalMSecs() ) -
                                       Pt::int64_t( second.time().totalMSecs() );

            Pt::int64_t result = (dayDiff * Time::MSECS_PER_DAY + milliSecDiff) * 1000;

            return result;
        }

    private:
        Date _date;
        Time _time;
};


inline void operator >>=(const SerializationInfo& si, DateTime& datetime)
{
    Date date(1,1,1);
    si.getMember("date") >>= date;
    datetime.setDate(date);

    Time time;
    si.getMember("time") >>= time;
    datetime.setTime(time);
}


inline void operator <<=(SerializationInfo& si, const DateTime& datetime)
{
    si.setTypeName("DateTime");

    SerializationInfo& date = si.addMember("date");
    date <<= datetime.date();

    SerializationInfo& time  = si.addMember("time");
    time <<= datetime.time();
}

}

#endif // PT_DATETIME_H
