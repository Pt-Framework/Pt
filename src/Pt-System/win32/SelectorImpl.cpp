/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Bjoern Oliver Streule                         *
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
#include "SelectorImpl.h"
#include "SelectableImpl.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/Selector.h"
#include "Pt/System/Application.h"
#include <algorithm>

namespace Pt {

namespace System {

SelectorImpl::SelectorImpl()
: _app(0)
{
    _current = _devices.end();

    _wakeEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( _wakeEvent == NULL )
        throw SystemError("CreateEvent failed", PT_SOURCEINFO);
        
    _ioEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( _ioEvent == NULL )
    {
        CloseHandle( _wakeEvent );
        throw SystemError("CreateEvent failed", PT_SOURCEINFO);
    }
    
    _handles.add( _wakeEvent, 0 );
    _handles.add( _ioEvent, 0 );
}


SelectorImpl::~SelectorImpl()
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


void SelectorImpl::add(Selectable& dev)
{
    _dirty.insert(&dev);
}


void SelectorImpl::remove(Selectable& dev)
{
    _avail.erase(&dev);
    _dirty.erase(&dev);        
    _handles.remove(dev);

    std::set<Selectable*>::iterator iter = _devices.find( &dev );
    if( iter == _devices.end() )
        return;
    
    if (_current == _devices.end())
    {
        _devices.erase(iter);
    }
    else if (*_current == *iter)
    {
        _devices.erase(_current++);
    }
    else
    {
        _devices.erase(iter);
    }
}


void SelectorImpl::wake()
{
    SetEvent( _wakeEvent );
}


void SelectorImpl::onEnabled(Selectable& s)
{

}


void SelectorImpl::onDisabled(Selectable& s)
{

}


void SelectorImpl::onStateChanged(Selectable& s)
{
    if( s.avail() )
    {
        _avail.insert(&s);
    }
    else
    {
        _avail.erase(&s);
    }
}


bool SelectorImpl::wait( unsigned umsecs )
{
    // convert unsigned to signed
    int msecs = umsecs;
    if(umsecs == Selector::WaitInfinite) 
    {
        msecs = INFINITE;
    }
    else if( umsecs > unsigned( std::numeric_limits<int>::max() ) )
    {
        msecs = std::numeric_limits<int>::max();
    }

    std::set<Selectable*>::iterator iter;
    for( iter = _dirty.begin(); iter != _dirty.end(); ++iter )
    {
        bool ready = false;
        bool accept = (*iter)->simpl().setWaitHandle(_ioEvent, ready);
        if(accept)
        {
            if(ready) 
                _avail.insert(*iter);

            _devices.insert(*iter);
        }
        else
        {
            accept = (*iter)->simpl().getWaitHandles(_handles);
        }
    }
    _dirty.clear();

    if( ! _avail.empty() )
    {
        SetEvent(_ioEvent);
    }
    _avail.clear();
    
    DWORD result = WaitForMultipleObjects( _handles.size(), _handles.handles(), false, msecs );
    if(result == WAIT_FAILED)
    {
        //DWORD err = GetLastError();
        throw IOError("WaitForMultipleObjects failed", PT_SOURCEINFO);
    }
    else if( result == WAIT_TIMEOUT ) 
    {
        return false;
    }

    bool avail = false;
    const Pt::ssize_t offset  = (result - WAIT_OBJECT_0);
    try
    {
        // wake event at offset 0 was active
        if (offset == 0)
        {
            return true;
        }
        // I/O event at offset 1 was active
        else if (offset == 1)
        {        
            bool avail = false;
            for( _current = _devices.begin(); _current != _devices.end(); )
            {
                Selectable* dev = *_current;
    
                if ( dev->enabled() && dev->simpl().checkEvent() )
                {
                    avail = true;
                }
    
                if (_current != _devices.end())
                {
                    if (*_current == dev)
                    {
                        ++_current;
                    }
                }
            }
    
            return avail;
        }
        else
        {
    	   Selectable* selectable = _handles.at(offset);
    	   return selectable->simpl().checkEvent();
    	}
    }
    catch (...)
    {
        _current = _devices.end();
        throw;
    }
    
    return avail;
}

} //namespace System

} //namespace Pt
