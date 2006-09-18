/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
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

#include "Pt/Api.h"
#include "Pt/System/Thread.h"

#include <windows.h>
#include <errno.h>


namespace Pt {

namespace System {

	class PT_EXPORT ThreadImpl {
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
			static DWORD WINAPI entry(void* arg)
			{
				ThreadImpl* impl = (ThreadImpl*)arg;
                impl->_thread.run();
				return 0;
			}

		protected:
			void close();

		private:
			Thread& _thread;
			HANDLE  _handle;
			DWORD  _id;
			Priority _priority;
			Thread::State _state;
			Thread::Mode _mode;
	};

} // namespace System

} // namespace Pt



