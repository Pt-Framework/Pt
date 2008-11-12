/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner , Sven Falk                      *
 *   Copyright (C) 2006 Sven Falk                                          *
 *   Copyright (C) 2006 Aloysius Indrayanto                                *
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

#ifndef Pt_NonCopyable_h
#define Pt_NonCopyable_h

#include <Pt/Api.h>


namespace Pt {

    /** @brief Protects derived classes from being copied
     *  @ingroup Pt
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
