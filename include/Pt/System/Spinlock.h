#ifndef PT_SYSTEM_SPINLOCK_H
#define PT_SYSTEM_SPINLOCK_H

#include <Pt/Api.h>
#include <Pt/AtomicInt.h>
#include <Pt/NonCopyable.h>


namespace Pt {

namespace System {

	class PT_EXPORT Spinlock : public NonCopyable {
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

			//! Aquire a spinlock
			inline void lock()
			{
				// busy loop until unlock
				while(_count.compareExchange(0, 1) ) {
					;
				}
			}

			//! Release a spinlock
			inline void unlock()
			{
				// set unlocked
				_count = 0;
			}

			//! @brief Only for testing. Do not use in production code
			bool testIsLocked() const
			{ return _count.value() != 0; }

	private:
		AtomicInt _count;
	};

} // !namespace System

} // !namespace Pt

#endif
