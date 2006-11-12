/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
 *   Copyright (C) 2005 Aloysius Indrayanto                                *
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

#ifndef Pt_Exception_h
#define Pt_Exception_h



#include <Pt/Api.h>

#include <exception>

#include <Pt/SourceInfo.h>

//#include <string>


namespace std
{
	class PT_API std::exception;
}

namespace Pt {

	//! @ingroup Pt
	//!	@brief This is the base Exception class for the Pt framework.
	/**
		This is the base class for all Exceptions used in this framework. This
		class implements std::exception and overrides std::exception::what()
		to return the error message passed at construction time. Besides an
		error message, Exceptions can provide information where it was
		raised in the source code through a SourceInfo object.

	*/
	class PT_EXPORT Exception : public std::exception {
		public:
			//! @brief Creates an exception from an error message and SourceInfo.
			/**
				Construction of an Exception object, requires a SourceInfo object as
			    argument. Use the PT_SOURCEINFO macro to pass SourceInfos to
			    exceptions:

			    @code
					throw MyException( "dammit!", PT_SOURCEINFO );
			    @endcode

			    @param what the error message
			    @param si location where the exception was raised
			*/
			Exception(const std::string& what, const SourceInfo& si) throw();

			//! @brief Copy constructor.
			Exception(const Exception& err) throw();

			//! @brief Destructor.
			~Exception() throw();

			//! @brief Returns information where the exception was raised
			const SourceInfo& sourceInfo() const throw();

			//! @brief Returns an error message
			const char* what() const throw();

			//! @brief Destructor.
			Exception& operator=(const Exception& err) throw();

		protected:
			std::string _what;
			SourceInfo  _source;
	};


	//! @ingroup Pt
	//!	@brief This is the base class for runtime errors.
	/**
		This is the base class for all runtime errors used in this framework.
		This class implements Exception.

	*/
	class PT_EXPORT RuntimeError : public Exception {
		public:
			//! @see Exception()
			RuntimeError(const std::string & what, const SourceInfo& si) throw();

			//! @brief Destructor.
			~RuntimeError() throw();
	};

	//! @ingroup Pt
	//!	@brief This is the base class for logic errors.
	/**
		This is the base class for all logic errors used in this framework.
		This class implements Exception.

	*/
	class PT_EXPORT LogicError : public Exception {
		public:
			//! @see Exception()
			LogicError(const std::string & what, const SourceInfo& si) throw();

			//! @brief Destructor.
			~LogicError() throw();
	};

	//! @ingroup Pt
	//!	@brief This class indicates an overflow error.
	/**
		An exception of class OverflowError is used to report an arithmetic
		overflow.
		This class implements Exception.

	*/
	class PT_EXPORT OverflowError : public RuntimeError {
		public:
			//! @see Exception()
			OverflowError(const std::string & what, const SourceInfo& si) throw();

			//! @brief Destructor.
			~OverflowError() throw();
	};

	//! @ingroup Pt
	//!	@brief This class indicates an underflow error.
	/**
		An exception of class UnderflowError is used to report an arithmetic
		underflow.
		This class implements Exception.

	*/
	class PT_EXPORT UnderflowError : public RuntimeError {
		public:
			//! @see Exception()
			UnderflowError(const std::string & what, const SourceInfo& si) throw();

			//! @brief Destructor.
			~UnderflowError() throw();
	};

	//! @ingroup Pt
	//!	@brief This class indicates a range error.
	/**
		An exception of class RangeError is used to report a range error
		in internal computations.
		This class implements Exception.

	*/
	class PT_EXPORT RangeError : public RuntimeError {
		public:
			//! @see Exception()
			RangeError(const std::string & what, const SourceInfo& si) throw();

			//! @brief Destructor.
			~RangeError() throw();
	};

	//! @ingroup Pt
	//!	@brief This indicates an illegal argument error.
	/**
		An exception of class �legalArgument is used to report invalid
		arguments e.g. if a bit set is initialized with a char other than
		'0' or '1'.
		This class implements Exception.

	*/
	class PT_EXPORT IllegalArgument : public LogicError {
		public:
			//! @see Exception()
			IllegalArgument(const std::string& what, const SourceInfo& si) throw();

			//! @brief Destructor.
			~IllegalArgument() throw();
	};

} // namespace Pt

#endif
