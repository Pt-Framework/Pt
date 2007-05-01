/***************************************************************************
 *   Copyright (C) 2004-2007 Marc Boris Dürner                             *
 *   Copyright (C) 2005,2007 by Jeroen van der Zijp.                       *
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
#ifndef PT_DATE_H
#define PT_DATE_H

#include <Pt/Api.h>
#include <Pt/Exception.h>


namespace Pt {

class InvalidDate : public std::invalid_argument
{
    public:
        InvalidDate(const SourceInfo& si);

        ~InvalidDate() throw();
};


/*
  Notes:
  - Henry F. Fliegel and Thomas C. Van Flandern, "A Machine Algorithm for
    Processing Calendar Dates". CACM, Vol. 11, No. 10, October 1968, pp 657.
*/
/** @brief Date expressed in year, month, and day
*/
class PT_API Date
{
    public:
        enum
        {
            Jan = 1, Feb, Mar,  Apr, May, Jun,
            Jul, Aug, Sep, Oct, Nov, Dec
        };

        enum
        {
            Sun = 0, Mon, Tue, Wed, Thu, Fri, Sat
        };

    public:
        /** \brief Default constructor.
        */
        Date()
        : _julian(0)
        {}

        /** \brief copy constructor.
        */
        Date(const Date& date)
        : _julian(date._julian)
        {}

        /** \brief Constructs a Date from given values

            Sets the date to a new year, month and day.
            InvalidDate is thrown if any of the values is out of range
        */
        Date(int y, int m, int d);

        /** \brief Constructs a Date from julian days
        */
        Date(unsigned j)
        : _julian(j)
        {}

        void setJulian(unsigned d)
        { _julian=d; }

        unsigned julian() const
        { return _julian; }

        /** \brief Sets the date.

            Sets the date to a new year, month and day.
            InvalidDate is thrown if any of the values is out of range
        */
        void set(int y, int m, int d);

        void get(int& y, int& m, int& d) const;

        /** \brief Returns the day-part of the date.
        */
        int day() const;

        /** \brief Returns the month-part of the date.
        */
        int month() const;

        /** \brief Returns the year-part of the date.
        */
        int year() const;

        //! @brief Return day of the week, starting with sunday
        int dayOfWeek() const;

        int daysInMonth() const;

        int dayOfYear() const;

        bool leapYear() const;

        // TODO: move to Pt:.System
        //static Date localDate();

        // TODO: move to Pt:.System
        //static Date universalDate();

        Date& operator=(const Date& date)
        { _julian=date._julian;return *this; }

        Date& operator+=(int x)
        { _julian+=x; return *this; }

        Date& operator-=(int x)
        { _julian-=x; return *this; }

        Date& operator++()
        { _julian++; return *this; }

        Date& operator--()
        { _julian--; return *this; }

        bool operator==(const Date& date) const
        { return _julian==date._julian; }

        bool operator!=(const Date& date) const
        { return _julian!=date._julian; }

        bool operator<(const Date& date) const
        { return _julian<date._julian; }

        bool operator<=(const Date& date) const
        { return _julian<=date._julian; }

        bool operator>(const Date& date) const
        { return _julian>date._julian; }

        bool operator>=(const Date& date) const
        { return _julian>=date._julian; }

        friend inline Date operator+(const Date& d,int x);

        friend inline Date operator+(int x,const Date& d);

        friend inline int operator-(const Date& a,const Date& b);

        /** \brief returns the date in ISO-format.

            Converts the date in ISO-format (yyyy-mm-dd).

            \return Date as iso formated string.
        */
        std::string toIsoString() const;

        /** \brief Interprets a string as a date-string in ISO-format

            Interprets a string as a date-string in ISO-format (yyyy-mm-dd) and
            returns a Date-object. When the string is not in ISO-format, an
            exception is thrown.

            \param Iso formated date string.
            \return Date result
            \throw IllegalArgument
        */
        static Date fromIsoString(const std::string& s);

    public:
        /** \brief Returns true if values describe a valid date.
        */
        static bool isValid(int y,int m,int d);

        static bool leapYear(int y);

        static void greg2jul(unsigned& jd,int y,int m,int d);

        // Converts julian days to a gregorian date
        static void jul2greg(unsigned jd,int& y,int& m,int& d);

    private:
        unsigned _julian;
};

inline Date operator+(const Date& d,int x)
{ return Date(d._julian+x); }

inline Date operator+(int x,const Date& d)
{ return Date(x+d._julian); }

inline int operator-(const Date& a,const Date& b)
{ return a._julian-b._julian; }

}

#endif
