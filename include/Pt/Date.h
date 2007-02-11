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

#ifndef PT_DATE_H
#define PT_DATE_H

#include <Pt/Api.h>

#include <string>


namespace Pt {

    /** \brief Chronological Date class.
        
        The date is used to contain the number of years, months and days.
    */
    class PT_API Date
    {
        protected:
            unsigned short _year;
            unsigned short _month;
            unsigned short _day;

        public:
            /** \brief Default constructor.
            
                Initializes the Date-object with empty values.
            */
            Date()
                : _year(0), 
                _month(0), 
                _day(0)
            { }

            /** \brief Initializes the date-object with the given values.
                
                No range-checks are done.
                
                \param year Year component of date.
                \param month Month component of date.
                \param day Day component of date.
            */
            Date(unsigned short year,
                unsigned short month,
                unsigned short day)
            : _year(year)
            , _month(month)
            , _day(day)
            { }

            /** \brief Returns the year-part of the date.
                
                Returns the year-part of the date.
                
                \return Year component of date.
            */
            unsigned short years() const     
            { return _year; }
            
            /** \brief Returns the month-part of the date.
            
                Returns the month-part of the date.
                
                \return Month component of date.
            */
            unsigned short months() const    
            { return _month; }
            
            /** \brief Returns the day-part of the date.
            
                Returns the day-part of the date.
            
                \return Day component of date.
            */
            unsigned short days() const      
            { return _day; }


            /** \brief Check if Date value is valid.
            
                Check if Date value is valid.
            
                \return True if invalid date.
            */
            bool isNull() const  
            { return _month == 0; }

            /** \brief Sets the date.
            
                Sets the date to a new year, month and day.
                No range-checks are done.

                \param year Year component of date.
                \param month Month component of date.
                \param day Day component of date.
            */
            void set(unsigned short year,
                unsigned short month,
                unsigned short day)
            {
                _year = year;
                _month = month;
                _day = day;
            }

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

            /** \brief Equality comparison operator
            
                Returns true if both dates have the same year, month and day.
            
                \param dt other Date
                \return true if equal
            */
            bool operator== (const Date& dt) const
            { 
                return _year == dt._year
                    && _month == dt._month
                    && _day == dt._day; }

            /** \brief Inequality comparison operator
            
                Returns true if the dates differ in year, month or day.
            
                \param dt other Date
                \return true if not equal
            */
            bool operator!= (const Date& dt) const
            { return !operator==(dt); }

            /** \brief Less-than comparison operator
            
                Returns true if the date is earier than the other date.
            
                \param dt other Date
                \return true if this date is  earlier
            */
            bool operator< (const Date& dt) const
            { 
                return _year < dt._year
                    || _year == dt._year
                    && ( _month < dt._month
                    || _month == dt._month
                    && _day < dt._day
                    ); }

            /** \brief Greater-than comparison operator
            
                Returns true if the date is later than the other date.
            
                \param dt other Date
                \return true if this date is later
            */
            bool operator> (const Date& dt) const
            { return dt < *this; }

            /** \brief Less-than or equal comparison operator
            
                Returns true if the date is earier or equal to the other date.
            
                \param dt other Date
                \return true if this date is earlier or equal
            */
            bool operator<= (const Date& dt) const
            { return !(*this > dt); }

            /** \brief Greater-than or equal comparison operator
            
                Returns true if the date is later or equal to the other date.
            
                \param dt other Date
                \return true if this date is later or equal
            */
            bool operator>= (const Date& dt) const
            { return !(*this < dt); }
    };
}

#endif // PT_DATE_H
