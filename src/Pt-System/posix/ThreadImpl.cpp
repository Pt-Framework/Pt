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

#include <unistd.h>
#include <signal.h>
#include <errno.h>


namespace Pt {

namespace System {


ThreadImpl::ThreadImpl(Thread& obj, Thread::Mode mode)
: _thread(obj),
  _id(0),
  _priority(InheritPriority),
  _state(Thread::Ready),
  _mode(mode)
{
}


ThreadImpl::~ThreadImpl()
{
}


void ThreadImpl::detach()
{
	if( !_id ) {
		return;
	}

	int ret = pthread_detach(_id);
	if( ret != 0 )
		throw SystemError("Could not detach thread. ", PT_SOURCEINFO);

	_mode = Thread::Detached;
}


void ThreadImpl::setPriority(Priority prio)
{
	_priority = prio;

	// only save new priority if thread is not running
	if(_state != Thread::Running) {
		return;
	}

	// TODO: InheritPriority ???

	sched_param sched;
	int policy = 0;
	pthread_getschedparam(_id, &policy, &sched);

	int priomin = sched_get_priority_min(policy);
	int priomax = sched_get_priority_max(policy);

	// currently we have 5 priorities, therefore divide range by 4
	int step = (priomax - priomin) / 4;
	int n = step * _priority;
	sched.sched_priority = std::min(priomax, priomin+n);

	if( 0 !=  pthread_setschedparam(_id, policy, &sched) )
		throw SystemError("Could not set priority.", PT_SOURCEINFO);
}


void ThreadImpl::start(Thread::Mode mode)
{
	size_t stacksize = 0;

	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
 	pthread_attr_setinheritsched(&attrs, PTHREAD_INHERIT_SCHED);

	if(stacksize > 0){
		pthread_attr_setstacksize(&attrs ,stacksize);
	}

	if(mode == Thread::Detached){
		pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
	}
	else {
		pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);
	}

	int ret = pthread_create(&_id, &attrs, this->entry, this);
	pthread_attr_destroy(&attrs);

	if(ret != 0) {
		_id = 0;
		throw SystemError("Could not create thread. ", PT_SOURCEINFO);
	}

	_state = Thread::Running;

	// setPriority() might have been called before start()
	if(_priority != InheritPriority) {
		this->setPriority(_priority);
	}
}


void ThreadImpl::wait()
{
	void* threadRet = 0;
	int ret = pthread_join(_id, &threadRet);

	if(ret != 0)
		throw SystemError("Could not join thread. ", PT_SOURCEINFO);

	_state = Thread::Finished;
	_id = 0;
}


void ThreadImpl::exit()
{
	::pthread_exit( NULL );
}


void ThreadImpl::terminate()
{
	int ret = pthread_kill(_id, SIGKILL);

	if(ret != 0)
		throw SystemError("Could not terminate thread. ", PT_SOURCEINFO);

	_state = Thread::Finished;
	_id = 0;
}


void ThreadImpl::yield()
{
	sched_yield();
}


void ThreadImpl::sleep(unsigned int ms)
{
	//struct timespec ts;
	//ts.tv_sec  = ms / 1000;
	//ts.tv_nsec = (ms % 1000) * 1000000;
	//pthread_delay(ts);

	usleep(ms * 1000);
}



}
}









