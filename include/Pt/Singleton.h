/*
 * Copyright (C) 2005-2013 by Marc Boris Duerner
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

#ifndef Pt_Singleton_h
#define Pt_Singleton_h

#include <Pt/NonCopyable.h>

namespace Pt {

    /** @brief %Singleton class template

        @param T Type of the singleton
        @param A Allocator for type T

        The Singleton class template can be used to easily implement the Singleton
        design pattern. It can either be used directly or as a base class. An
        allocator can be used to control how the singleton instance will be
        allocated.

        The follwing example shows how to use the singleton as a base class:
        @code
              class MySingleton : public Singleton<MySingleton>
              {
                   friend class Singleton<MySingleton>;

                   // ...
               };
        @endcode

       @ingroup CoreTypes
     */
    template <typename T>
    class Singleton : public NonCopyable
    {
        public:
            /** @brief Returns the instance of the singleton type

                    When called for the first time, the singleton instance will be
                    created with the specified alloctaor. All subsequent calls wikk
                    return a reference to the previously created instance.

                @return The singleton instance
             */
            static T& instance()
            {
                if( !_instance )
                {
                    _instance = create();
                }

                return *_instance;
            }

        protected:
            /**  @brief Constructor
             */
            Singleton()
            { }

            /**  @brief Destructor
             */
            ~Singleton()
            { }

            static T* create()
            {
                static T inst;
                return &inst;
            }

        private:
            static T* _instance;
    };

    template <typename T>
    T* Singleton<T>::_instance = 0;

} // namespace Pt

#endif
