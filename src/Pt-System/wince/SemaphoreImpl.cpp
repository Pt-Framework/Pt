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

#include "SemaphoreImpl.h"
#include "Pt/System/Semaphore.h"
#include "Pt/System/SystemError.h"

#include <climits>


namespace Pt {

namespace System {


SemaphoreImpl::SemaphoreImpl(Semaphore& sem, unsigned int initial)
: _sem(sem)
{
	_handle = CreateSemaphore(NULL, initial, LONG_MAX, 0);

	if( !_handle ) {
		SystemError error("Could not create semaphore.", PT_SOURCEINFO);
		_sem.raiseError( error );
		return;
	}
}


SemaphoreImpl::~SemaphoreImpl()
{
	CloseHandle(_handle);
}


void SemaphoreImpl::wait()
{
	DWORD ret = WaitForSingleObject(_handle, INFINITE);
	if(ret == WAIT_FAILED){
		SystemError error("Could not wait on semaphore.", PT_SOURCEINFO);
		_sem.raiseError( error );
		return;
	}
}


bool SemaphoreImpl::tryWait()
{
	DWORD ret = WaitForSingleObject(_handle, 0);
	if(ret == WAIT_FAILED) {
		SystemError error("Could not wait on semaphore.", PT_SOURCEINFO);
		_sem.raiseError( error );
		return false;
	}
	else if(ret == WAIT_OBJECT_0) {
		return true;
	}

	return false;
}


void SemaphoreImpl::post()
{
	if( 0 == ReleaseSemaphore(_handle, 1, NULL) ) {
		SystemError error("Could not post semaphore.", PT_SOURCEINFO);
		_sem.raiseError( error );
		return;
	}
}


} // namespace System;

} // namespace Ptv
