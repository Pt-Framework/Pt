/*
 * Copyright (C) 2006-2013 Marc Boris Duerner
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

#ifndef Pt_NonCopyable_h
#define Pt_NonCopyable_h

#include <Pt/Api.h>

namespace Pt {

/** @brief Base class that disables copy construction and assignment.

    Inherit privately from %NonCopyable when a type must not be copied.
    The copy constructor and assignment operator are private, so a
    derived class cannot copy or assign, and neither can a caller.
    The result is a compile-time error, not a run-time failure.

    @code
    class MyClass : private Pt::NonCopyable
    {
        // ...
    };
    @endcode

    Private inheritance is the usual form: %NonCopyable is not part of
    the public interface. Public inheritance also prevents copies, but
    it exposes the mixin as a base. Do not try to make a type
    copyable again in a further derived class; the private members of
    %NonCopyable stay private. Types that need to be moved but not
    copied still inherit here and define their own move operations.

    @ingroup Pt-Core
*/
class NonCopyable {
    public:
        /** @brief Default constructor.
        */
        NonCopyable()
        { }

        /** @brief Destructor.
        */
        ~NonCopyable()
        { }

    private:
        /** @brief No copy constructor
        */
        NonCopyable(const NonCopyable&);

        /** @brief No assignment operator
        */
        NonCopyable& operator=(const NonCopyable&);
};

} // namespace Pt

#endif
