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


namespace Pt {

class SerializationNode;
class SerializationData;

/** @brief Combined %Date and %Time value
    @ingroup DateTime
*/
class PT_API DateTime
{
    //friend PT_API const Archive& operator>>(const Archive&, DateTime& );
    //friend PT_API Archive& operator<<(Archive&, const DateTime& );

    public:
        DateTime();

        DateTime(int year, unsigned month, unsigned day,
                 unsigned hour = 0, unsigned min = 0, unsigned sec = 0, unsigned msec = 0);

        DateTime(unsigned julianDay);

        DateTime(const DateTime& dateTime);

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
            Pt::int64_t totalMSecs = ts.totalMSecs();
            Pt::int64_t days = totalMSecs / Time::MSecsPerDay;

            Pt::int64_t overrun = totalMSecs % Time::MSecsPerDay;
            if( overrun + _time.totalMSecs() > Time::MSecsPerDay)
                days += 1;

            _date += static_cast<int>(days);

            _time += Timespan(overrun);
            return *this;
        }

        /** @brief Assignment by difference operator
        */
        DateTime& operator-=(const Timespan& ts)
        {
            Pt::int64_t totalMSecs = ts.totalMSecs();
            Pt::int64_t days = totalMSecs / Time::MSecsPerDay;

            Pt::int64_t overrun = totalMSecs % Time::MSecsPerDay;
            if( overrun > _time.totalMSecs() )
                days += 1;
            _date -= static_cast<int>(days);

            _time -= Timespan( overrun );
            return *this;
        }

    private:
        Date _date;
        Time _time;
};

PT_API const SerializationNode& operator>>(const SerializationNode& data, DateTime& time);

PT_API SerializationData& operator<<(SerializationData& data, const DateTime& time);

}

#endif // PT_DATETIME_H
