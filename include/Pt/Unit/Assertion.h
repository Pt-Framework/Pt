/*
 * Copyright (C) 2005-2008 by Dr. Marc Boris Duerner
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
#ifndef PT_UNIT_ASSERTION_H
#define PT_UNIT_ASSERTION_H

#include <Pt/Unit/Api.h>
#include "Pt/SourceInfo.h"
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <limits>

namespace Pt {

namespace Unit {

    /** @brief %Test %Assertion exception

        Assertions are modeled as an exception type, which is thrown by Unit
        tests when an assertion has failed. This class implements
        std::exception and overrides std::exception::what() to return an
        error message Besides the error message, Assertions can provide
        information where the exception was raised in the source code through
        a SourceInfo object. It is recommended to use the PT_UNIT_ASSERT
        for easy creation from a source info object.

        @code
            void myTest()
            {
                int ten = 5 + 5;
                PT_UNIT_ASSERT(ten == 10)
            }
        @endcode

        @ingroup unittest
    */
    class PT_UNIT_API Assertion
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
            Assertion(const std::string& what, const SourceInfo& si);

            /** @brief Information where assertion failed.
            */
            const Pt::SourceInfo& sourceInfo() const;

            const char* what() const  { return _what.c_str(); }

        private:
            Pt::SourceInfo _sourceInfo;
            std::string _what;
    };

    namespace { bool testCond = false; }

    inline bool getFalse()
    { return testCond; }

    #define PT_UNIT_ASSERT(cond) \
        do { \
            if( !(cond) ) \
                throw Pt::Unit::Assertion(#cond, PT_SOURCEINFO); \
        } while (::Pt::Unit::testCond)

    #define PT_UNIT_ASSERT_MSG(cond, what) \
        do { \
            if( !(cond) ) \
            { \
                std::ostringstream _pt_msg; \
                _pt_msg << what; \
                throw Pt::Unit::Assertion(_pt_msg.str(), PT_SOURCEINFO); \
            } \
        } while (::Pt::Unit::testCond)

    // TODO: deprecated
    #define PT_UNIT_ASSERT_EQUALS(value1, value2) \
        do { \
            if( ! ((value1) == (value2)) ) \
            { \
                std::ostringstream _pt_msg; \
                _pt_msg << "not equal: value1 (" #value1 ")=<" << value1 << "> value2 (" #value2 ")=<" << value2 << '>'; \
                throw Pt::Unit::Assertion(_pt_msg.str(), PT_SOURCEINFO); \
            } \
        } while (::Pt::Unit::testCond)

    #define PT_UNIT_ASSERT_EQUAL(value1, value2) \
        do { \
            if( ! ((value1) == (value2)) ) \
            { \
                std::ostringstream _pt_msg; \
                _pt_msg << "not equal: (" #value1 ")=<" << value1 << ">, (" #value2 ")=<" << value2 << '>'; \
                throw Pt::Unit::Assertion(_pt_msg.str(), PT_SOURCEINFO); \
            } \
        } while (::Pt::Unit::testCond)

    #define PT_UNIT_ASSERT_NEAR(value1, value2)                                                            \
        do {                                                                                              \
            const double _pt_v1      = static_cast<double>(value1);                                      \
            const double _pt_v2      = static_cast<double>(value2);                                      \
            const double _pt_rel_eps = std::sqrt(std::numeric_limits<double>::epsilon());                \
            const double _pt_abs_eps = std::numeric_limits<double>::min();                               \
            const double _pt_scale   = std::fmax(std::fabs(_pt_v1), std::fabs(_pt_v2));                  \
            const double _pt_eps     = std::fmax(_pt_rel_eps * _pt_scale, _pt_abs_eps);                  \
            if( std::fabs(_pt_v1 - _pt_v2) > _pt_eps )                                                  \
            {                                                                                             \
                std::ostringstream _pt_msg;                                                               \
                _pt_msg << "not near: (" #value1 ")=<" << _pt_v1                                         \
                        << ">, (" #value2 ")=<" << _pt_v2                                                \
                        << ">, eps=<" << _pt_eps                                                          \
                        << ">, diff=<" << std::fabs(_pt_v1 - _pt_v2) << '>';                             \
                throw Pt::Unit::Assertion(_pt_msg.str(), PT_SOURCEINFO);                                  \
            }                                                                                             \
        } while (::Pt::Unit::testCond)

    #define PT_UNIT_ASSERT_THROW(cond, EX) \
        do { \
            struct _pt_ex { }; \
            try \
            { \
                cond; \
                throw _pt_ex(); \
            } \
            catch(const _pt_ex &) \
            { \
                std::ostringstream _pt_msg; \
                _pt_msg << "exception of type " #EX " expected in " #cond; \
                throw Pt::Unit::Assertion(_pt_msg.str(), PT_SOURCEINFO); \
            } \
            catch(const EX &) \
            {} \
        } while (::Pt::Unit::testCond)

    #define PT_UNIT_ASSERT_NOTHROW(cond) \
        do { \
            try { \
            \
                cond; \
            } \
            catch(const std::exception& e) \
            { \
                throw Pt::Unit::Assertion( \
                    std::string("unexpected exception of type ") + typeid(e).name() + ": " + e.what(), \
                    PT_SOURCEINFO); \
            } \
            catch(...) \
            { \
                throw Pt::Unit::Assertion("unexpected exception." , PT_SOURCEINFO); \
            } \
        } while (::Pt::Unit::testCond)

    #define PT_UNIT_FAIL(what) \
        do { \
            std::ostringstream _pt_msg; \
            _pt_msg << what; \
            throw Pt::Unit::Assertion(_pt_msg.str(), PT_SOURCEINFO); \
        } while (::Pt::Unit::testCond)

} // namespace

} // namespace

#endif  // include guard
