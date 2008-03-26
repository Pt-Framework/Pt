/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
 *   Copyright (C) 2006 by Roman Schnider                                  *
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
#ifndef Pt_System_MutexLock_h
#define Pt_System_MutexLock_h

#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>


namespace Pt {

namespace System {

    /** @brief MutexLock class for Mutex.

        The MutexLock class adds functionality for scoped
        locking. In the constructor of a  MutexLock, the mutex is locked
        and in the destructor it is unlocked. This way if for example an
        exception occures in the protected section the Mutex will be unlocked
        during stack unwinding when the MutexLock is destructed.

        @code
             // example how to make a member function thread-safe
             #include <Pt/System/Mutex.h>

             class MyClass
             {
                 public:
                     void function()
                     {
                         MutexLock lock(_lock);

                         //
                         // protected operations
                         //

                        // dtor of MutexLock unlocks _lock
                     }

                 private:
                     Pt::System::Mutex _lock;
             };
        @endcode
    */
    class MutexLock {
        public:
            //! @brief Constructor
            /**
                Construct a MutexLock object and lock the enclosing mutex.

                \param m the enclosing Mutex object
            */
            MutexLock(Mutex& m)
            : _mutex(m)
            , _isLocked(true)
            { _mutex.lock(); }

            //! @brief Destructor
            /**
                The destructor unlocks the mutex.
             */
            ~MutexLock()
            {
                try
                {
                    if(_isLocked)
                        _mutex.unlock();
                }
                catch(...) {}
            }

            //! @brief Unlock so that the destructor does not unlock
            void unlock()
            {
                _mutex.unlock();
                _isLocked = false;
            }

             //! @brief Get the mutex object
             /**
                 \return the enclosing Mutex object
              */
            Mutex& mutex()
            { return _mutex; }

             //! @brief Get the mutex object
             /**
                 \return the enclosing Mutex object
              */
            const Mutex& mutex() const
            { return _mutex; }

            private:
                Mutex& _mutex;
                bool _isLocked;
    };


} // !namespace System

} // !namespace Pt

#endif
