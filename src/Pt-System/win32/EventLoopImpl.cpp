/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2008 Marc Boris Duerner
 * Copyright (C) 2006-2007 Bjoern Oliver Streule
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
#include "EventLoopImpl.h"
#include "SelectableImpl.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/EventLoop.h"
#include "Pt/System/Application.h"
#include <algorithm>
#include <limits>

namespace Pt {

namespace System {

EventLoopImpl::EventLoopImpl()
: _app(0)
{
    _current = _devices.end();
    _currentAvail = _avail.end();

    _wakeEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( _wakeEvent == NULL )
        throw SystemError( PT_ERROR_MSG("CreateEvent failed") );

    _ioEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( _ioEvent == NULL )
    {
        CloseHandle( _wakeEvent );
        throw SystemError( PT_ERROR_MSG("CreateEvent failed") );
    }

    _handles.add( _wakeEvent, 0 );
    _handles.add( _ioEvent, 0 );
}


EventLoopImpl::~EventLoopImpl()
{
    std::set<Selectable*>::iterator it;
    while( _devices.size() )
    {
        it = _devices.begin();
        (*it)->setSelector(0);
    }

    while( _dirty.size() )
    {
        it = _dirty.begin();
        (*it)->setSelector(0);
    }

    while( _handles.size() )
    {
        Selectable* s = _handles.at(0);
        if( s )
            s->setSelector(0);
        else
            _handles.pop_front();
    }

    CloseHandle( _wakeEvent );
    CloseHandle( _ioEvent );
}


void EventLoopImpl::add(Selectable& s)
{
    _dirty.insert(&s);
}


void EventLoopImpl::remove(Selectable& s)
{
    _dirty.erase(&s);
    _handles.remove(s);

    std::set<Selectable*>::iterator iter = _devices.find( &s );
    if( iter == _devices.end() )
        return;

    if( _current != _devices.end() &&
       *_current == *iter )
        _devices.erase(_current++);
    else
        _devices.erase(iter);

    iter = _avail.find( &s );
    if( iter == _avail.end() )
        return;

    if( _currentAvail != _avail.end() &&
       *_currentAvail == *iter )
        _avail.erase(_currentAvail++);
    else
        _avail.erase(iter);
}


void EventLoopImpl::changed(Selectable& s)
{
    if( s.avail() )
    {
        _avail.insert(&s);
    }
    else
    {
        std::set<Selectable*>::iterator it = _avail.find( &s );
        if( it == _avail.end() )
            return;

        if( _currentAvail != _avail.end() &&
           *_currentAvail == *it )
            _avail.erase(_currentAvail++);
        else
            _avail.erase(it);
    }
}


void EventLoopImpl::wake()
{
    SetEvent( _wakeEvent );
}


WaitResult EventLoopImpl::waitNext( std::size_t umsecs )
{
    // convert unsigned to signed
    DWORD msecs = umsecs;
    if(umsecs == EventLoop::WaitInfinite)
    {
        msecs = INFINITE;
    }
    else if( umsecs > std::numeric_limits<DWORD>::max() )
    {
        msecs = std::numeric_limits<DWORD>::max();
    }

    std::set<Selectable*>::iterator iter;
    for( iter = _dirty.begin(); iter != _dirty.end(); ++iter )
    {
        bool ready = false;
        bool accept = (*iter)->simpl().setWaitHandle(_ioEvent, ready);
        if(accept)
            _devices.insert(*iter);

        (*iter)->simpl().getWaitHandles(_handles, ready);

        if(ready)
            _avail.insert(*iter);
    }
    _dirty.clear();

    if( _avail.size() )
    {
        msecs = 0;
    }

    DWORD result = WaitForMultipleObjects( _handles.size(), _handles.handles(), false, msecs );
    if(result == WAIT_FAILED)
    {
        //DWORD err = GetLastError();
        throw IOError( PT_ERROR_MSG("WaitForMultipleObjects failed") );
    }

    WaitResult ret;
    try
    {
        // check all selectables that did not require waiting
        for( _currentAvail = _avail.begin(); _currentAvail != _avail.end(); )
        {
            Selectable* s = *_currentAvail;
            if( s->enabled() && s->simpl().checkEvent() )
            {
                //avail = true;
                ret.setDevice();
            }
            if( _currentAvail != _avail.end() &&
               *_currentAvail == s )
            {
                    ++_currentAvail;
            }
        }

        if( result == WAIT_TIMEOUT)
        {
            return ret;
        }

        const Pt::ssize_t offset = (result - WAIT_OBJECT_0);

        // wake event at offset 0 was active
        if (offset == 0)
        {
            return ret.setEvent();
        }
        // I/O event at offset 1 was active
        else if (offset == 1)
        {
            for( _current = _devices.begin(); _current != _devices.end(); )
            {
                Selectable* dev = *_current;
                if ( dev->enabled() && dev->simpl().checkEvent() )
                {
                    //avail = true;
                    ret.setDevice();
                }

                if( _current != _devices.end() &&
                   *_current == dev )
                {
                    ++_current;
                }
            }
        }
        else
        {
            Selectable* selectable = _handles.at(offset);
            if( selectable->enabled() && selectable->simpl().checkEvent() )
                //avail = true;
                ret.setDevice();

        }
    }
    catch (...)
    {
        _current = _devices.end();
        _currentAvail = _avail.end();
        throw;
    }

    return ret;
}

} //namespace System

} //namespace Pt
