/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *                                                                         *
 ***************************************************************************/

#include "Pt/Api.h"
#include "Pt/System/Thread.h"

#include <windows.h>

#ifndef _WIN32_WCE
	#include <process.h>
#endif


namespace Pt {

namespace System {

#ifdef _WIN32_WCE
	typedef DWORD threadid_t;
#else
	typedef  unsigned threadid_t;
#endif

	class ThreadImpl {
		public:
			enum Priority
            {
				LowestPriority  = 0,
				LowPriority     = 1,
				NormalPriority  = 2,
				HighPriority    = 3,
				HighestPriority = 4,
				InheritPriority = 5
			};

		public:
			ThreadImpl(Thread& obj, Thread::Mode mode);

			~ThreadImpl();

			Thread::Mode mode() const
			{ return _mode; }

			Thread::State state() const
			{ return _state; }

			void start(Thread::Mode mode);

			void detach();

			void wait();

			void setPriority(Priority prio);

			Priority priority() const
			{ return _priority; }

			static void exit() throw();

			void terminate();

			static void yield() throw();

			static void sleep(unsigned int ms) throw();

		public:
			static threadid_t WINAPI entry(void* arg)
			{
				ThreadImpl* impl = (ThreadImpl*)arg;
                impl->_thread.run();
                impl->_state = Thread::Finished;
				return 0;
			}

		protected:
			void close();

		private:
			Thread& _thread;
			HANDLE  _handle;
			threadid_t _id;
			Priority _priority;
			Thread::State _state;
			Thread::Mode _mode;
	};


} // namespace System

} // namespace Pt



