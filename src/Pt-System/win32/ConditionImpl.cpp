/*
 * Copyright (C) 2006
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "ConditionImpl.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>

namespace Pt {

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

} // namespace System

} // namespace Pt
