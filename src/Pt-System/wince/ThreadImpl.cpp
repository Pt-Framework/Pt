/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Drner                               *
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

#include "ThreadImpl.h"

#include "Pt/System/SystemError.h"


namespace Pt {

namespace System {


ThreadImpl::ThreadImpl(Thread& obj, Thread::Mode mode)
: _thread(obj),
  _handle(0),
  _id(0),
  _priority(Thread::InheritPriority),
  _state(Thread::Ready),
  _mode(mode)
{
}


ThreadImpl::~ThreadImpl()
{
	this->close();
}


void ThreadImpl::close()
{
	if (_handle != 0) {
		::CloseHandle(_handle);
		_handle = 0;
	}
}


void ThreadImpl::setPriority(Thread::Priority prio)
{
	_priority = prio;

	// only save new priority if thread is not running
	if(_state != Thread::Running) {
		return;
	}

	int winPrio = THREAD_PRIORITY_NORMAL;
	switch(_priority) {
		case Thread::LowestPriority:
			winPrio = THREAD_PRIORITY_LOWEST;
			break;
		case Thread::LowPriority:
			winPrio = THREAD_PRIORITY_BELOW_NORMAL;
			break;
		case Thread::NormalPriority:
			winPrio = THREAD_PRIORITY_NORMAL;
			break;
		case Thread::HighPriority:
			winPrio = THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		case Thread::HighestPriority:
			winPrio = THREAD_PRIORITY_HIGHEST;
			break;
	}

	// TODO: InheritPriority ???

	if(0 == ::SetThreadPriority(_handle, winPrio) ) {
		SystemError error("Could not set priority.", PT_SOURCEINFO);
		_thread.raiseError( error );
		return;
	}
}


void ThreadImpl::start(Thread::Mode mode) {
	SIZE_T stackSize = 0;

	_handle = ::CreateThread(NULL,
	                         stackSize,
	                         entry,
	                         this,
	                         0,
	                         &_id);

	if(_handle == NULL) {
		_id = 0;
		SystemError te("Could not create thread", PT_SOURCEINFO);
		_thread.raiseError( te );
		return;
	}

	_state = Thread::Running;

	// setPriority() might have been called before start()
	if(_priority != Thread::InheritPriority) {
		this->setPriority(_priority);
	}

	if(_mode == Thread::Detached) {
		this->close();
	}
}


void ThreadImpl::detach()
{
	// simply close the thread control handle.
	this->close();
	_mode = Thread::Detached;
}


void ThreadImpl::wait()
{
	DWORD status = ::WaitForSingleObject(_handle, INFINITE);
	if( status != WAIT_OBJECT_0 ) {
		SystemError te("Could not join thread", PT_SOURCEINFO);
		_thread.raiseError( te );
		return;
	}

	_state = Thread::Finished;
	_id = 0;
}


void ThreadImpl::exit() throw()
{
	DWORD status = 0;
	::ExitThread(status);
}


void ThreadImpl::terminate()
{
	if( !TerminateThread(_handle, 0) ) {
		SystemError te("Could not kill thread.", PT_SOURCEINFO);
		_thread.raiseError( te );
		return;
	}

	_state = Thread::Finished;
	_id = 0;
}


void ThreadImpl::yield() throw()
{
	::Sleep(0);
}


void ThreadImpl::sleep(unsigned int ms) throw()
{
	::Sleep(ms);
}


} // namespace System

} // namespace Ptv









