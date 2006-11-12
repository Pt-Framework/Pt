/***************************************************************************
 *   Copyright (C) 2005-2006 PTV AG                                        *
 ***************************************************************************/

#ifndef PT_SYSTEM_SPINLOCK_H
#define PT_SYSTEM_SPINLOCK_H

#include <Pt/Api.h>
#include <Pt/AtomicInt.h>
#include <Pt/NonCopyable.h>


namespace Pt {

namespace System {

    //! @brief Spnilock class.
	/**
    *  The most lightweight synchronisation object is the Spinlock. It is
    *  usually implemented with a status variable that can be set to “Locked”
    *  and “Unlocked” and atomic operations to change and inspect the status.
    *  When Spinlock::lock is called, the status is changed to “Locked”.
    *  Subsequent calls of Spinlock::lock from other threads will block until
    *  the first thread has called Spinlock::unlock and the state of
    *  the Spinlock has changed to “Unlocked”. Note that Spinlocks are not recursive.
    *  When a Spinlock::lock blocks a busy-wait happens, therefore a Spinlock is only
    *  usable in cases where resources need to be locked for a very short time, but in
    *  these cases a higher performance can be achieved.
    */
	class PT_API Spinlock : public NonCopyable {
		public:
			//! Lock class for Spinlock.
			class Lock {
				public:
					Lock(Spinlock& s)
					: _spinlock(s)
					{ _spinlock.lock(); }

					~Lock()
					{ _spinlock.unlock(); }

					Spinlock& spinlock()
					{ return _spinlock; }

					const Spinlock& spinlock() const
					{ return _spinlock; }

				private:
					Spinlock& _spinlock;
			};

		public:
			//! Default Constructor.
			Spinlock()
				: _count(0)
			{}

			//! Destructor.
			~Spinlock()
			{}


            //! @brief Lock.
			/// Locks the Spinlock. If the Spinlock is currently locked
            /// by another thread, the calling thread suspends until no
            /// other thread holds a lock on it. This happens
            /// performing a  busy-wait. Spinlocks are not recursive
            /// locking it multiple times before unlocking it is undefined.
			inline void lock()
			{
				// busy loop until unlock
				while(_count.compareExchange(0, 1) ) {
					;
				}
			}

            //! @brief Unlock.
			/// Unlocks the Spinlock.
			inline void unlock()
			{
				// set unlocked
				_count = 0;
			}

			bool testIsLocked() const
			{ return _count.value() != 0; }

	private:
		AtomicInt _count;
	};

} // !namespace System

} // !namespace Pt

#endif
