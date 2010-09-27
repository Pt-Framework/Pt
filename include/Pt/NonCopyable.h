/*
 * Copyright (C) 2006 Marc Boris Duerner , Sven Falk
 * Copyright (C) 2006 Sven Falk
 * Copyright (C) 2006 Aloysius Indrayanto
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

    /** @brief Protects derived classes from being copied
     *
     *  The NonCopyable class has a private copy constructor and assignment
     *  operator, therefore derived classes cannot be copied. This class
     *  is meant to be used as a mix-in class as shown in the code example
     *  below.
     *
     *  @code
     *      class MyClass : public NonCopyable {
     *            // ...
     *            // The class' implementation
     *            // ...
     *        };
     *  @endcode
     *
     *    Trying to copy a NonCopyable object will cause compile-time error.
     *  @ingroup Pt
     */
    class  NonCopyable {
        public:
            /** @brief Default constructor
            */
            NonCopyable()
            { }

            /** @brief Destructor
            */
            ~NonCopyable()
            { }

        private:
            /** @brief Declared as private to prevent usage of copy constructor
            */
            NonCopyable(const NonCopyable&); // No need to really implement it
            //{ }

            /**  @brief Declared as private to prevent usage of assignment operator
            */
            NonCopyable& operator=(const NonCopyable&); // No need to really implement it
            //{ return *this; }
    };

}

#endif
