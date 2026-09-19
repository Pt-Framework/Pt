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

/** @brief Process-wide single instance of @a T.

    %Singleton<T> owns one @a T for the process. %instance() creates
    that object on the first call and returns the same object on every
    later call. The instance is a function-local static created by
    %create(); it lives until the program exits.

    Derive from %Singleton<T> with @a T equal to the derived class,
    and befriend the base so it can construct @a T:

    @code
    class MySingleton : public Pt::Singleton<MySingleton>
    {
        friend class Pt::Singleton<MySingleton>;

    protected:
        MySingleton()
        { }
    };
    @endcode

    The derived constructor stays protected so callers cannot build a
    second instance. %instance() is the only public way to get the
    object. %Singleton is %NonCopyable, so the instance cannot be
    copied either.

    Construction is not synchronized. The first call to %instance()
    must happen before other threads call it, or the program must
    otherwise guarantee a single initializing call. There is no
    destructor hook and no way to replace the instance. Use this type
    for a process-wide service that is created on demand, not for
    objects whose lifetime the caller must control.

    @ingroup Pt-Core
*/
template <typename T>
class Singleton : public NonCopyable
{
    public:
        /** @brief Returns the process-wide instance, creating it on first call.
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
        /** @brief Constructor
          */
        Singleton()
        { }

        /** @brief Destructor
          */
        ~Singleton()
        { }

        //! @internal
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
