/*
 * Copyright (C) 2006-2012 Marc Boris Duerner
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
#include "Selector.h"
#include "Pt/System/SystemError.h"
#include <algorithm>
#include <limits>
#include <cassert>

namespace Pt {

namespace System {

Selector::Selector()
{
    _wakeEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( _wakeEvent == NULL )
        throw SystemError("CreateEvent failed");

    _handles.push_back( _wakeEvent );
}


Selector::~Selector()
{ 
    while( ! _selectables.empty() )
    {
        _selectables.first()->detach();
    }

    CloseHandle( _wakeEvent );
}


void Selector::attach(Selectable& s)
{
    _selectables.insert(s);
}


void Selector::detach(Selectable& s)
{
    SelectableList::unlink(s);
}



void Selector::wake()
{
    SetEvent( _wakeEvent );
}


bool Selector::waitForWake(size_t timeoutUMSecs)
{
    // convert unsigned to signed
    DWORD msecs = timeoutUMSecs;
    if(timeoutUMSecs == EventLoop::WaitInfinite)
    {
        msecs = INFINITE;
    }
    else if( timeoutUMSecs > std::numeric_limits<DWORD>::max() )
    {
        msecs = std::numeric_limits<DWORD>::max();
    }

    HANDLE* handles = _handles.size() > 0 ? &_handles[0] : 0;

    bool isTimeout = false;
    DWORD offset = waitFor(_handles.size(), handles, msecs, isTimeout);

    try
    {
        if(isTimeout)
            return false;

        // wake event at offset 0 was active
        if (offset == 0)
        {
            return true;
        }
    }
    catch (...)
    {
        throw;
    }

    return false;
}


DWORD Selector::waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout)
{
    DWORD result = WaitForMultipleObjects( numHandles, handles, false, msecs );
    if(result == WAIT_FAILED)
    {
        throw SystemError("WaitForMultipleObjects");
    }

    if( result == WAIT_TIMEOUT)
    {
        isTimeout = true;
        return 0;
    }

    return result - WAIT_OBJECT_0;
}

}

}

