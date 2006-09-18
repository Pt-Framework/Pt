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

#include "SemaphoreImpl.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/Semaphore.h"


namespace Pt {

namespace System {

SemaphoreImpl::SemaphoreImpl(unsigned int initial)
{
	_handle = CreateSemaphore(NULL, initial, LONG_MAX, 0);

	if( !_handle )
		throw SystemError("Could not create semaphore.", PT_SOURCEINFO);
}


SemaphoreImpl::~SemaphoreImpl()
{
	CloseHandle(_handle);
}


void SemaphoreImpl::wait()
{
	DWORD ret = WaitForSingleObject(_handle, INFINITE);
	if(ret == WAIT_FAILED)
		throw SystemError("Could not wait on semaphore.", PT_SOURCEINFO);
}


bool SemaphoreImpl::tryWait()
{
	DWORD ret = WaitForSingleObject(_handle, 0);
	if(ret == WAIT_FAILED) {
		throw SystemError("Could not wait on semaphore.", PT_SOURCEINFO);
	}
	else if(ret == WAIT_OBJECT_0) {
		return true;
	}

	return false;
}


void SemaphoreImpl::post()
{
	if( 0 == ReleaseSemaphore(_handle, 1, NULL) )
		throw SystemError("Could not post semaphore.", PT_SOURCEINFO);
}


} // namespace System;

} // namespace Pt
