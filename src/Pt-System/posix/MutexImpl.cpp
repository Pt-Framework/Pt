/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
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

#include "MutexImpl.h"

#include "Pt/System/SystemError.h"
#include "Pt/System/Mutex.h"

#include <sys/time.h>
#include <errno.h>


namespace Pt {

namespace System {


MutexImpl::MutexImpl(Mutex& mutex)
: _mutex(mutex)
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&_handle, &attr);
}


MutexImpl::~MutexImpl()
{
	pthread_mutex_destroy(&_handle);
}


void MutexImpl::lock()
{
	int ret = pthread_mutex_lock(&_handle);
	if(ret != 0)
		throw SystemError("Could not lock mutex: ", PT_SOURCEINFO);
}


bool MutexImpl::tryLock(unsigned int msec)
{
	int ret = 0;

	// get start time
	struct timeval start, current;
	::gettimeofday(&start, 0);
	start.tv_sec = start.tv_sec + msec / 1000;
	start.tv_usec = (start.tv_usec + (msec % 1000) * 1000);

	for( ; ; ) {
		ret = pthread_mutex_trylock(&_handle);

		if( (ret != EBUSY && ret != ETIMEDOUT) )
			break;

		// check if timeout
		::gettimeofday(&current, 0);
		if( current.tv_sec > start.tv_sec ||
				(current.tv_sec == start.tv_sec && current.tv_usec > start.tv_usec) )
			break;
	}

	switch(ret) {
		case 0:         break;
		case EBUSY:     return false;
		case ETIMEDOUT: return false;
		default: {
			throw SystemError("Could not lock mutex: ", PT_SOURCEINFO);
		}
	}

	return true;
}


void MutexImpl::unlock()
{
	int ret = pthread_mutex_unlock(&_handle);
	if(ret != 0)
		throw SystemError("Could not unlock mutex: ", PT_SOURCEINFO);
}


} // !namepsace System

} // !namespace Pt
