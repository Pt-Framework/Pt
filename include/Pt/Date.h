/*
 * Copyright (C) 2004-2008 Marc Boris Duerner
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

#ifndef PT_DATE_H
#define PT_DATE_H

#include <Pt/Api.h>
#include <Pt/String.h>
#include <string>
#include <stdexcept>

namespace Pt {

class SerializationInfo;

/** @brief Indicates an invalid date value.

    This exception is thrown, when a invalid values are used for a date,
    such as a value greater than twelve for the month. 

    @ingroup DateTime
*/
class PT_API InvalidDate : public std::runtime_error
{
    public:
        //! @brief Constructor.
        InvalidDate();

        //! @brief Destructor.
        ~InvalidDate() throw()
        {}
};

//! @internal
PT_API std::string dateToString(const Date& date);

//! @internal
PT_API Date dateFromString(const std::string& s);

//! @internal
PT_API void greg2jul(unsigned& jd, int y, int m, int d);

//! @internal
PT_API void jul2greg(unsigned jd, int& y, int& m, int& d);

/** @brief %Date expressed in year, month, and day.

    @ingroup DateTime
*/
class Date
{
    public:
        enum Month
        {
            Jan = 1, Feb, Mar,  Apr, May, Jun,
            Jul, Aug, Sep, Oct, Nov, Dec
        };

        enum WeekDay
        {
            Sun = 0, Mon, Tue, Wed, Thu, Fri, Sat
        };

        /**
        * @brief The number of days of an ordinary year.
        */
        static const unsigned DaysPerYear = 365;

        /**
        * @brief The number of days of a leap year.
        */
        static const unsigned DaysPerLeapYear = 366;

        /**
        * @brief The number of days of a January.
        */
        static const unsigned DaysOfJan = 31;

        /**
        * @brief The number of days of a February.
        */
        static const unsigned DaysOfFeb = 28;

        /**
        * @brief The number of days of a February in a leap year.
        */
        static const unsigned DaysOfLeapFeb = 29;

        /**
        * @brief The number of days of a March.
        */
        static const unsigned DaysOfMar = 31;

        /**
        * @brief The number of days of a April.
        */
        static const unsigned DaysOfApr = 30;

        /**
        * @brief The number of days of a May.
        */
        static const unsigned DaysOfMay = 31;

        /**
        * @brief The number of days of a June.
        */
        static const unsigned DaysOfJun = 30;

        /**
        * @brief The number of days of a July.
        */
        static const unsigned DaysOfJul = 31;

        /**
        * @brief The number of days of a August.
        */
        static const unsigned DaysOfAug = 31;

        /**
        * @brief The number of days of a September.
        */
        static const unsigned DaysOfSep = 30;

        /**
        * @brief The number of days of a October.
        */
        static const unsigned DaysOfOct = 31;

        /**
        * @brief The number of days of a November.
        */
        static const unsigned DaysOfNov = 30;

        /**
        * @brief The number of days of a December.
        */
        static const unsigned DaysOfDec = 31;

    public:
        /** \brief Default constructor.

            The default constructed date is undefined.
        */
        Date()
        : _julian(0)
        {}

        /** \brief Copy constructor.
        */
        Date(const Date& date)
        : _julian(date._julian)
        {}

        /** \brief Constructs a Date from given values

            Sets the date to a new year, month and day.
            InvalidDate is thrown if any of the values is out of range
        */
        Date(int y, unsigned m, unsigned d)
        : _julian(0)
        {
            greg2jul(_julian, y, m, d);
        }

        /** \brief Constructs a Date from a julian day
        */
        explicit Date(unsigned julianDays)
        : _julian(julianDays)
        {}

        /** @brief Assignment operator
        */
        Date& operator=(const Date& date)
        { _julian = date._julian; return *this; }

        /** @brief Sets the Date to a julian day
        */
        void setJulian(unsigned d)
        { _julian=d; }

        /** @brief Returns the Date as a julian day
        */
        unsigned julian() const
        { return _julian; }

        /** \brief Sets the date to a year, month and day

            Sets the date to a new year, month and day.
            InvalidDate is thrown if any of the values is out of range
        */
        void set(int y, unsigned m, unsigned d)
        {
            greg2jul(_julian, y, m, d);
        }

        /** @brief Gets the year, month and day
        */
        void get(int& year, unsigned& month, unsigned& day) const;

        /** \brief Returns the day-part of the date.
        */
        unsigned day() const;

        /** \brief Returns the month-part of the date.
        */
        unsigned month() const;

        /** \brief Returns the year-part of the date.
        */
        int year() const;

        /** @brief Return day of the week, starting with sunday
        */
        unsigned dayOfWeek() const;

        /** @brief Returns the days of the month of the date
        */
        unsigned daysInMonth() const;

        /** @brief Returns the day of the year
        */
        unsigned dayOfYear() const;

        /** @brief Returns true if the date is in a leap year
        */
        bool isLeapYear() const;

        /** \brief Returns the date in ISO-format

            Converts the date in ISO-format (yyyy-mm-dd).

            \return Date as iso formated string.
        */
        std::string toIsoString() const
        { return dateToString(*this); }

        /** \brief Interprets a string as a date-string in ISO-format

            Interprets a string as a date-string in ISO-format (yyyy-mm-dd) and
            returns a Date-object. When the string is not in ISO-format, an
            exception is thrown.

            \param s Iso formated date string.
            \return Date result
            \throw IllegalArgument
        */
        static Date fromIsoString(const std::string& s)
        { return dateFromString(s); }

        /** @brief Add days to the date
        */
        Date& operator+=(int days)
        { _julian += days; return *this; }

        /** @brief Substract days from the date
        */
        Date& operator-=(int days)
        { _julian -= days; return *this; }

        /** @brief Increments the date by one day
        */
        Date& operator++()
        { _julian++; return *this; }

        /** @brief Decrements the date by one day
        */
        Date& operator--()
        { _julian--; return *this; }

        /** \brief Returns true if values describe a valid date
        */
        static bool isValid(int y, int m, int d);

        /** @brief Returns true if the year is in a leap year
        */
        static bool isLeapYear(int year);

    private:
        //! @internal
        unsigned _julian;
};


PT_API void operator >>=(const SerializationInfo& si, Date& date);


PT_API void operator <<=(SerializationInfo& si, const Date& date);


/** @brief Returns true if the dates are equal.

    @related Date
*/
inline bool operator==(const Date& a, const Date& b)
{ return a.julian() == b.julian(); }

/** @brief Returns true if the dates are not equal.

    @related Date
*/
inline bool operator!=(const Date& a, const Date& b)
{ return a.julian() != b.julian(); }

/** @brief Less-than comparison operator.

    @related Date
*/
inline bool operator<(const Date& a, const Date& b)
{ return a.julian() < b.julian(); }

/** @brief Less-than-equal comparison operator.

    @related Date
*/
inline bool operator<=(const Date& a, const Date& b)
{ return a.julian() <= b.julian(); }

/** @brief Greater-than comparison operator.

    @related Date
*/
inline bool operator>(const Date& a, const Date& b)
{ return a.julian() > b.julian(); }

/** @brief Greater-than-equal comparison operator.

    @related Date
*/
inline bool operator>=(const Date& a, const Date& b) 
{ return a.julian() >= b.julian(); }


inline Date operator+(const Date& d, int days)
{ return Date(d.julian() + days); }


inline Date operator+(int days, const Date& d)
{ return Date(days + d.julian()); }


inline int operator-(const Date& a, const Date& b)
{ return a.julian() - b.julian(); }


inline void Date::get(int& y, unsigned& m, unsigned& d) const
{
    int mon, day_;
    jul2greg(_julian, y, mon, day_);
    m = mon;
    d = day_;
}


inline bool Date::isLeapYear(int y)
{
    return ((y%4==0) && (y%100!=0)) || (y%400==0);
}


inline unsigned Date::day() const
{
    int d,m,y;
    jul2greg(_julian, y ,m, d);
    return d;
}


inline unsigned Date::month() const
{
    int d,m,y;
    jul2greg(_julian, y, m, d);
    return m;
}


inline int Date::year() const
{
    int d,m,y;
    jul2greg(_julian, y, m, d);
    return y;
}


inline unsigned Date::dayOfWeek() const
{
    return (_julian+1) % 7;
}


inline unsigned Date::daysInMonth() const
{
    static const unsigned char monthDays[13]=
    {
        0,31,28,31,30,31,30,31,31,30,31,30,31
    };

    int y, m, d;
    jul2greg(_julian, y, m, d);

    if( m==2 && isLeapYear(y) )
        return 29;

    return monthDays[m];
}


inline unsigned Date::dayOfYear() const
{
    int y,m,d;
    unsigned jd;
    jul2greg(_julian,y,m,d);

    greg2jul(jd,y,1,1);
    return _julian-jd+1;
}


inline bool Date::isLeapYear() const
{
    int d,m,y;
    jul2greg(_julian,y,m,d);
    return isLeapYear(y);
}


inline bool Date::isValid(int, int m, int d)
{
    if(m<1 || m>12 || d<1 || d>31)
    {
        return false;
    }

    return true;
}

} // namespace Pt

#endif // PT_DATE_H
