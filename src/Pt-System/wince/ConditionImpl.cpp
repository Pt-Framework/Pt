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
#include "ConditionImpl.h"


namespace Ptv {

namespace System {


ConditionImpl::ConditionImpl()
: _blockCount(0)
{
	// event to wake one, autoreset
	_event1 = CreateEvent(NULL, 0, 0, NULL);

	// event to wake all, manual reset
	_event2 = CreateEvent(NULL, 1, 0, NULL);

	InitializeCriticalSection( &_critSec );
}


ConditionImpl::~ConditionImpl()
{
	CloseHandle(_event1);
	CloseHandle(_event2);
	DeleteCriticalSection(&_critSec);
}


bool ConditionImpl::wait(Mutex& mtx, unsigned int ms )
{
	EnterCriticalSection(&_critSec);
	_blockCount++;
	LeaveCriticalSection(&_critSec);
	mtx.unlock();

	HANDLE handles[2];
	handles[0] = _event1;
	handles[1] = _event2;

	if( ms == size_t(-1) )
		ms = INFINITE;

	DWORD result = WaitForMultipleObjects(2, handles, 0, ms);

	EnterCriticalSection(&_critSec);
	_blockCount--;

	// Unblocked by broadcast and no other blocked threads
	int last_waiter = (result == WAIT_OBJECT_0 + 1) && (_blockCount == 0);
	LeaveCriticalSection(&_critSec);

	if(last_waiter)
		ResetEvent( _event2 );

	mtx.lock();

	return result != WAIT_TIMEOUT;
}


void ConditionImpl::signal()
{
	EnterCriticalSection(&_critSec);
	bool blocked = (_blockCount > 0 );
	LeaveCriticalSection( &_critSec );

	if(blocked)
		SetEvent( _event1 );
}


void ConditionImpl::broadcast()
{
	EnterCriticalSection(&_critSec);
	bool blocked = ( _blockCount > 0 );
	LeaveCriticalSection( &_critSec );

	if(blocked)
		SetEvent( _event2 );
}

}

}
