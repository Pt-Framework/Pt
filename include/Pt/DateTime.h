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

#ifndef PT_DATETIME_H
#define PT_DATETIME_H

#include <Pt/Time.h>
#include <Pt/Date.h>
#include <Pt/Api.h>

#include <string>
#include <limits>


namespace Pt {

    /**    \brief This class holds a date and time.
    */
    class PT_API DateTime
    {
        private:
            Date _date;
            Time _time;
            
        public:
    
        /** \brief Default constructor.

            Initializes the Date-object with empty values.
        */    
        DateTime()
            :    _date()
            ,    _time()
        {}
    
        /** \brief Construct a date-time from a date and time.

            Initializes the date-time object from a given date and time.
        */        
        DateTime(const Date& date, const Time& time)
            :    _date(date)
            ,    _time(time)
        {}
    
        /** \brief Returns the hour-part of the date-time.
                    
            Returns the hour-part of the date-time.
            
            \return Hour component of date-time.
        */
        unsigned short hours() const     
        { return _time.hours(); }

        /** \brief Returns the minute-part of the date-time.
                    
            Returns the minute-part of the date-time.
            
            \return Minute component of date-time.
        */
        unsigned short minutes() const   
        { return _time.minutes(); }

        /** \brief Returns the second-part of the date-time.
                    
            Returns the second-part of the date-time.
            
            \return Second component of date-time.
        */
        unsigned short seconds() const   
        { return _time.seconds(); }

        /** \brief Returns the millisecond-part of the date-time.
                    
            Returns the millisecond-part of the date-time.
            
            \return Millisecond component of date-time.
        */
        unsigned short msecs() const   
        { return _time.msecs(); }
        
        /** \brief Returns the year-part of the date-time.
            
            Returns the year-part of the date-time.
            
            \return Year component of date-time.
        */
        unsigned short years() const     
        { return _date.years(); }
        
        /** \brief Returns the month-part of the date-time.
        
            Returns the month-part of the date-time.
            
            \return Month component of date-time.
        */
        unsigned short months() const    
        { return _date.months(); }
        
        /** \brief Returns the day-part of the date-time.
        
            Returns the day-part of the date-time.
        
            \return Day component of date-time.
        */
        unsigned short days() const      
        { return _date.days(); }
        
        /** \brief Check if date-time value is valid.
        
            Check if date-time value is valid.
        
            \return True if invalid date-time.
        */
        bool isNull() const  
        { return _date.isNull(); }

        /** \brief Sets the date-time.
        
            Sets the date-time to a new year, month, day, hour, minute, second, milli-second.
            No range-checks are done.

            \param year Year component of date-time.
            \param month Month component of date-time.
            \param day Day component of date-time.
            \param hour Hour component of date-time.
            \param minute Minute component of date-time.
            \param second Second component of date-time.
            \param msec Milli-Second component of date-time.
        */
        void set(unsigned short year,
            unsigned short month,
            unsigned short day,
            unsigned short hour,
            unsigned short minute,
            unsigned short second,
            unsigned short msec = 0)
        {
            _date.set(year, month, day);
            _time.set(hour, minute, second, msec);
        }
                
        /** \brief Returns the date-time in iso-format.
        
            Converts the date-time in iso-format (yyyy-mm-dd hh:mm:ss.hhh).
            \return Date-time as iso formated string.
        */
        std::string toIsoString() const;
        
        /** \brief Interprets a string as a date-time-string in iso-format 
        
            Interprets a string as a date-time-string in iso-format (yyyy-mm-dd hh:mm:ss.hhh)
            and returns a DateTime-object. When the string is not in iso-format, an 
            exception is thrown.
            
            \param Iso formated date-time string.
            \return DateTime result
            \throw IllegalArgument
        */
        static DateTime fromIsoString(const std::string& s);
        
        /** \brief Equality comparison operator
        
            Returns true if both date-times have the same year, month, day, hour, minute, second, milli-second.
        
            \param dt other date-time
            \return true if equal
        */
        bool operator== (const DateTime& dt) const
        { 
            return _date == dt._date
                && _time == dt._time;
        }

        /** \brief Inequality comparison operator
        
            Returns true if the date-times differ in year, month, day, hour, minute, second, milli-second.
        
            \param dt other date-time
            \return true if not equal
        */
        bool operator!= (const DateTime& dt) const
        { return !operator==(dt); }

        /** \brief Less-than comparison operator
        
            Returns true if the date-time is earier than the other date-time.
        
            \param dt other date-time
            \return true if this date-time is earlier
        */
        bool operator< (const DateTime& dt) const
        { 
            return this->years() < dt.years()
                    || this->years() == dt.years()
                    && ( this->months() < dt.months()
                    || this->months() == dt.months()
                        && ( this->days() < dt.days()
                        || this->days() == dt.days()
                        && ( this->hours() < dt.hours()
                            || this->hours() == dt.hours()
                            && ( this->minutes() < dt.minutes()
                                || this->minutes() == dt.minutes()
                                && ( this->seconds() < dt.seconds()
                                || this->seconds() == dt.seconds()
                                    && this->msecs() < dt.msecs()
                                )
                            )
                        )
                    )
            ); 
        }

        /** \brief Greater-than comparison operator
        
            Returns true if the date-time is later than the other date-time.
        
            \param dt other date-time
            \return true if this date-time is later
        */
        bool operator> (const DateTime& dt) const
        { return dt < *this; }

        /** \brief Less-than or equal comparison operator
        
            Returns true if the date-time is earier or equal to the other date-time.
        
            \param dt other date-time
            \return true if this date-time is earlier or equal
        */
        bool operator<= (const DateTime& dt) const
        { return !(*this > dt); }

        /** \brief Greater-than or equal comparison operator
        
            Returns true if the date-time is later or equal to the other date-time.
        
            \param dt other date-time
            \return true if this date-time is later or equal
        */
        bool operator>= (const DateTime& dt) const
        { return !(*this < dt); }
                
    };

}

#endif // PT_DATETIME_H
