/***************************************************************************
 *   Copyright (C) 2006                                                    *
 ***************************************************************************/
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
