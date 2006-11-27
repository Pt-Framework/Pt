/***************************************************************************
 *   Copyright (C) 2006 by Tommi Maekitalo                                 *
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
 *   Copyright (C) 2006 by Stefan Bueder                                   *
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

#include <string>
#include <limits>


#undef max


namespace Pt {

	/**	\brief This class holds a time.
	*/
	class PT_API Time
	{
		protected:
			unsigned short _hour;
			unsigned short _minute;
			unsigned short _second;
			unsigned short _msec;

	public:
		/** \brief Creates a Time-object with null values.
		*/
		Time()
			: _hour( std::numeric_limits<unsigned short>::max() ),
			_minute(0),
			_second(0),
			_msec(0)
		{ }

		/** \brief Creates a Time-object from given values.

			No range-checks are done.

			\param hour hours
			\param minute minutes
			\param second seconds
			\param msec milli-seconds
		*/
		Time(unsigned short hour,
			unsigned short minute,
			unsigned short second,
			unsigned short msec = 0)
		: _hour(hour)
		, _minute(minute)
		, _second(second)
		, _msec(msec)
		{ }

		/** \brief Returns the hour-part of the Time.
		*/
		unsigned short hours() const
		{ return _hour; }

		/** \brief Returns the minute-part of the Time.
		*/
		unsigned short minutes() const
		{ return _minute; }

		/** \brief Returns the second-part of the Time.
		*/
		unsigned short seconds() const
		{ return _second; }

		/** \brief Returns the millisecond-part of the Time.
		*/
		unsigned short msecs() const
		{ return _msec; }

		/** \brief Check if Time value is valid
		*/
		bool isNull() const
		{ return _hour == std::numeric_limits<unsigned short>::max(); }

		/** \brief Sets the time.

			Sets the time to a new hour, minute, second, milli-second.
			No range-checks are done.

			\param hour Hour component of time.
			\param minute Minute component of time.
			\param second Second component of time.
			\param msec Milli-Second component of time.
		*/
		void set(unsigned short hour,
			unsigned short minute,
			unsigned short second,
			unsigned short msec = 0)
		{
			_hour = hour;
			_minute = minute;
			_second = second;
			_msec = msec;
		}

		/** \brief Returns the time in ISO-format (hh:mm:ss.hhh)
		*/
		std::string toIsoString() const;

		/** \brief Convert from an ISO time string

			Interprets the passed string as a time-string in ISO-format
			(hh:mm:ss.hhh) and returns a Time-object. When the string is not
			in ISO-format, an exception is thrown.

			\param s time as iso-string (hh:mm:ss.hhh)
			\return Time object
			\throw IllegalArgument xxx
		*/
		static Time fromIsoString(const std::string& s);

		/** \brief Equality comparison operator

			Returns true if both times are equal.
			\param dt time object
			\return true if equal
		*/
		bool operator==(const Time& dt) const
		{
			return _hour == dt._hour
				&& _minute == dt._minute
				&& _second == dt._second
				&& _msec == dt._msec;
		}

		/** \brief Inequality comparison operator

			Returns true if times are different.
			\param dt time object
			\return true if not equal
		*/
		bool operator!= (const Time& dt) const
		{ return !operator==(dt); }

		/** \brief Less-than comparison operator

			Returns true if this time is earlier.
			\param dt time object
			\return true if earlier
		*/
		bool operator< (const Time& dt) const
		{
			return _hour < dt._hour
				|| _hour == dt._hour
				&& ( _minute < dt._minute
				|| _minute == dt._minute
				&& ( _second < dt._second
				|| _second == dt._second
				&& _msec < dt._msec
				)
				);
		}

		/** \brief Greater-than comparison operator

			Returns true if this time is later.
			\param dt time object
			\return true if later
		*/
		bool operator> (const Time& dt) const
		{ return dt < *this; }

		/** \brief Less-than-equal comparison operator

			Returns true if this time is earlier or equal.
			\param dt time object
			\return true if earlier or equal
		*/
		bool operator<= (const Time& dt) const
		{ return !(*this > dt); }

		/** \brief Greater-than-equal comparison operator

			Returns true if this time is later or equal.
			\param dt time object
			\return true if later or equal
		*/
		bool operator>= (const Time& dt) const
		{ return !(*this < dt); }
	};
}

#endif // PT_TIME_H
