/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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
#ifndef Pt_System_Mutex_h
#define Pt_System_Mutex_h

#include <Pt/System/Api.h>
#include <Pt/NonCopyable.h>


namespace Pt {

namespace System {

    //! /brief Mutex synchronization object
    ///
    ///    A Mutex is a mutual exclusion device. It is used to synchronize
    ///    the access to data which is accessed by more than one thread or
    ///    process at the same time. Mutexes are recursive, that is the
    ///    same thread can lock a mutex multiple times without deadlocking.
    ///    When unlocking the mutex, unlock() must be called for each time
    ///    a thread has successfully called lock() or tryLock().
    class PT_SYSTEM_API Mutex : public NonCopyable {
        friend class MutexImpl;

        private:
            //! Implementation
            class MutexImpl* _impl;

        public:
            //! @brief Enumeration to select recursive or non-recursive mode.
            /// 
            /// Mutex can be created either as recursive or as non-recursive Mutex.
            /// A Mutex to be used in conjunction within the Condition-Class has to be 
            /// non-recursive.
            enum Mode
            {
                Normal = 0,
                Recursive  = 1
            };
            //! @brief Default constructor
            ///
            /// Construct the Mutex object.
            explicit Mutex(Mode mode);

            //! Destructor
            ///
            /// The destructor destroys the mutex. The mutex must be in unlocked
            /// state when the destructor is called.
            ~Mutex();

            //! @brief Lock the mutex
            ///
            /// Locks the mutex. If the mutex is currently locked by another
            /// thread, the calling thread suspends until no other thread holds
            /// a lock on it. If the mutex is already locked by the calling
            /// thread the function returns immediatly with incrementing the lock-
            /// count of the mutex. This prevents a thread from dead-locking while
            /// waiting for a mutex it already owns. To release its ownership under
            /// such circumstances the thread must unlock the mutex once for each
            /// time the thread has locked the mutex.
            void lock();

            //! @brief Unlock the mutex
            ///
            /// Unlocks the mutex. If the mutex was locked more than one time by the
            /// same thread unlock decrements the lock-count. The mutex is actually
            /// unlocked when the lock-count is zero.
            void unlock();

            //! @brief Returns if Mutex is recursive or non-recursive
            Mode mode() const
            { return _mode;}

           //! @brief Access to platform specific implementation
            MutexImpl* impl()
            { return _impl; }

    private:
            Mode _mode;
    };

} // !namespace System

} // !namespace Pt

#endif
