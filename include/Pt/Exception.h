/***************************************************************************
 *   Copyright (C) 2004-2006 Marc Boris Dürner                             *
 *   Copyright (C) 2005-2006 Aloysius Indrayanto                           *
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

#include <exception>

#include <Pt/Api.h>
#include <Pt/SourceInfo.h>


namespace std {
	class PT_API std::exception;
}


namespace Pt {

	/** @brief This is the base Exception class for the Pt framework.
	 *  @ingroup Pt
	 *
	 *  This is the base class for all Exceptions used in this framework. This
	 *  class implements std::exception and overrides std::exception::what()
	 *  to return the error message passed at construction time. Besides the
	 *  error message, Exceptions can provide information about where the
	 *  exception was raised in the source code through a SourceInfo object.
	 */
	class PT_API Exception : public std::exception {
		public:
			/** @brief Construct an Exception from an error message and SourceInfo.
			 *
			 *  @param what the error message
			 *  @param si location where the exception was raised
			 *
			 *  Construction of an Exception object requires a SourceInfo object as
			 *  argument. Use the PT_SOURCEINFO macro to pass SourceInfo to Exception:
			 *  @code
			 *  throw MyException( "dammit!", PT_SOURCEINFO );
			 *  @endcode
			 */
			Exception(const std::string& what, const SourceInfo& si) throw();

			/** @brief Copy constructor.
			 */
			Exception(const Exception& err) throw();

			/** @brief Destructor.
			 */
			~Exception() throw();

			/** @brief Returns the error message.
			 */
			const char* what() const throw();

			/** @brief Returns information where the exception was raised.
			 */
			const SourceInfo& sourceInfo() const throw();

			/** @brief Assigment operator.
			 */
			Exception& operator=(const Exception& err) throw();

		protected:
			std::string _what;
			SourceInfo  _source;
	};


	/** @brief This is the base class for all runtime errors.
	 *  @ingroup Pt
	 *
	 *  This is the base class for all runtime errors used in this framework.
	 *  This class implements Exception.
	 */
	class PT_API RuntimeError : public Exception {
		public:
			/** @see Exception().
			 */
			RuntimeError(const std::string & what, const SourceInfo& si) throw();

			/** @brief Destructor.
			 */
			~RuntimeError() throw();
	};


	/** @brief This is the base class for all logic errors.
	 *  @ingroup Pt
	 *
	 *  This is the base class for all logic errors used in this framework.
	 *  This class implements Exception.
	 */
	class PT_API LogicError : public Exception {
		public:
			/** @see Exception().
			 */
			LogicError(const std::string & what, const SourceInfo& si) throw();

			/** @brief Destructor.
			 */
			~LogicError() throw();
	};


	/** @brief This class indicates an overflow error.
	 *  @ingroup Pt
	 *
	 *  An exception of class OverflowError is used to report an arithmetic overflow.
	 *  This class implements Exception.
	 */
	class PT_API OverflowError : public RuntimeError {
		public:
			/** @see Exception().
			 */
			OverflowError(const std::string & what, const SourceInfo& si) throw();

			/** @brief Destructor.
			 */
			~OverflowError() throw();
	};


	/** @brief This class indicates an underflow error.
	 *  @ingroup Pt
	 *
	 *  An exception of class UnderflowError is used to report an arithmetic underflow.
	 *  This class implements Exception.
	 */
	class PT_API UnderflowError : public RuntimeError {
		public:
			/** @see Exception().
			 */
			UnderflowError(const std::string & what, const SourceInfo& si) throw();

			/** @brief Destructor.
			 */
			~UnderflowError() throw();
	};


	/** @brief This class indicates a range error.
	 *  @ingroup Pt
	 *
	 *  An exception of class RangeError is used to report a range error in internal
	 *  computations. This class implements Exception.
	 */
	class PT_API RangeError : public RuntimeError {
		public:
			/** @see Exception().
			 */
			RangeError(const std::string & what, const SourceInfo& si) throw();

			/** @brief Destructor.
			 */
			~RangeError() throw();
	};


	/** @brief This class indicates an illegal argument error.
	 *  @ingroup Pt
	 *
	 *  An exception of class IllegalArgument is used to report invalid	arguments
	 *  (e.g. if a bit set is initialized with a char other than '0' or '1').
	 *  This class implements Exception.
	 */
	class PT_API IllegalArgument : public LogicError {
		public:
			/** @see Exception().
			 */
			IllegalArgument(const std::string& what, const SourceInfo& si) throw();

			/** @brief Destructor.
			 */
			~IllegalArgument() throw();
	};


    /** @brief This indicates that a resource could not be accessed.
        @ingroup ptv

        An exception of class AccessError is used to report failed access
        to a resource due to missing authorization, mising access rights
        or if a resource is in an otherwise inaccessible state.
        This class implements Exception.
    */
    class PT_API AccessError : public LogicError {
        public:
            //! @see Exception()
            AccessError(const std::string& what, const SourceInfo& si) throw();

            //! @brief Destructor.
            ~AccessError() throw();
    };
} // namespace Pt

#endif
