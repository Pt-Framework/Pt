/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Drner                      *
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
#ifndef PTV_UNIT_ASSERTION_H
#define PTV_UNIT_ASSERTION_H

#include <Pt/Exception.h>
#include <iostream>


namespace Pt {

namespace Unit {

    /** @brief %Test %Assertion exception

        Assertions are modeled as an exception type, which is thrown by Unit
        tests when an assertion has failed. It is recommended to use the
        PT_UNIT_ASSERT for easy creation from a source info object.

        @code
            void myTest()
            {
                int ten = 5 + 5;
                PT_UNIT_ASSERT(ten == 10)
            }
        @endcode
    */
    class Assertion : public std::exception
    {
        public:
            /** @brief Construct from a message and source info.

                Constructs a assertion exception from a message string
                and a source info object that describes where the
                assertion failed. Use the PT_UNIT_ASSERT macro instead
                of this constructor.

                @param what Error message
                @param si Info where the assertion failed
            */
            Assertion(const std::string& what, const SourceInfo& si)
            : _what(what)
            , _sourceInfo(si)
            {}

            /** @brief Copy constructor.

                @param a Other asstertion exception
            */
            //Assertion(const Assertion& a)
            //: Exception(a)
            //{}

            const Pt::SourceInfo& sourceInfo() const
            { return _sourceInfo; }

            /** @brief Destructor.
            */
            ~Assertion() throw()
            {}

            const char* what() const throw()
            { return (_what + _sourceInfo).c_str(); }

        private:
            std::string _what;
            Pt::SourceInfo _sourceInfo;
    };

    #define PT_UNIT_ASSERT(cond) if( !(cond) ) throw Pt::Unit::Assertion(#cond, PT_SOURCEINFO);

} // namespace Unit

} // namespace Pt

#endif  // PTV_UNIT_ASSERTION_H
