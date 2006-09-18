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
#include "MutexImpl.h"

#include "Pt/System/SystemError.h"
#include "Pt/System/Mutex.h"


namespace Pt {

namespace System {


MutexImpl::MutexImpl(Mutex& mutex)
: _mutex(mutex)
{
	_handle = CreateMutex(NULL, FALSE, NULL);

	if( !_handle )
		throw SystemError("Could not create mutex: ", PT_SOURCEINFO);
}


MutexImpl::~MutexImpl()
{
	::CloseHandle(_handle);
}


void MutexImpl::lock()
{
	DWORD ret = WaitForSingleObjectEx(_handle, INFINITE, FALSE);

	if(ret != WAIT_OBJECT_0)
		throw SystemError ("Could not wait for mutex: ", PT_SOURCEINFO);
}


bool MutexImpl::tryLock(unsigned int msec)
{
	DWORD ret = WaitForSingleObjectEx(_handle, msec, FALSE);

	if(ret == WAIT_FAILED) {
		throw SystemError ("Could not wait for mutex: ", PT_SOURCEINFO);
	}
	else if(ret == WAIT_OBJECT_0)
		return true;

	return false;
}


void MutexImpl::unlock()
{
	if( !ReleaseMutex(_handle) )
		throw SystemError("Could not release mutex: ", PT_SOURCEINFO);
}


}

}
