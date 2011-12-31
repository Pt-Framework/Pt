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
#include "MainLoopImpl.h"
#include "SelectableImpl.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/EventLoop.h"
#include "Pt/System/Application.h"
#include <algorithm>
#include <limits>
#include <cassert>

namespace Pt {

namespace System {

EventLoopImpl::EventLoopImpl()
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

    _signalledEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( _signalledEvent == NULL )
    {
        CloseHandle( _wakeEvent );
        CloseHandle( _ioEvent );
        throw SystemError( PT_ERROR_MSG("CreateEvent failed") );
    }

    _handles.add( _wakeEvent, 0 );
    _handles.add( _ioEvent, 0 );
    _handles.add( _signalledEvent, 0 );
}


EventLoopImpl::~EventLoopImpl()
{ 
    CloseHandle( _wakeEvent );
    CloseHandle( _ioEvent );
    CloseHandle( _signalledEvent );
}

HANDLE EventLoopImpl::beginWait(Selectable& s)
{ 
    _devices.insert(&s);
    return _ioEvent; 
}


IOHandle* EventLoopImpl::registerHandle(Selectable& s, HANDLE h)
{
    IOHandle* iohandle =  new IOHandle(s, h);
    _dirty.push_back(iohandle);
    return iohandle;
}


void EventLoopImpl::unregisterHandle(IOHandle* h)
{
    _dirty.remove(h);
    _handles.remove( *(h->sel) );
    delete h;
}


void EventLoopImpl::setAvail(Selectable& s)
{
    _avail.insert(&s);
}


void EventLoopImpl::endWait(Selectable& s)
{
    std::set<Selectable*>::iterator iter = _devices.find( &s );
    if( iter != _devices.end() )
    {
        if( _current != _devices.end() && *_current == *iter )
        {
            _devices.erase(_current++);
        }
        else
        {
            _devices.erase(iter);
        }
    }

    iter = _avail.find( &s );
    if( iter != _avail.end() )
    {
        if( _currentAvail != _avail.end() && *_currentAvail == *iter )
            _avail.erase(_currentAvail++);
        else
            _avail.erase(iter);
    }
}


void EventLoopImpl::idle(Selectable& s)
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


void EventLoopImpl::avail(Selectable& s)
{
    _avail.insert(&s);
}


void EventLoopImpl::signalCancel(Selectable& s)
{
    Pt::System::MutexLock lock(_signalledMutex);
    _signalled.erase(&s);
}


void EventLoopImpl::signalAvail(Selectable& s)
{
    Pt::System::MutexLock lock(_signalledMutex);
    _signalled.insert(&s);
    SetEvent( _signalledEvent );
}


void EventLoopImpl::onRun()
{
    bool isActive = true;
    while(isActive)
    {
        size_t timeout = this->processTimers();

        this->waitNext(timeout, isActive);
    }
}


void EventLoopImpl::onWake()
{
    SetEvent( _wakeEvent );
}


void EventLoopImpl::waitNext(std::size_t umsecs, bool& isActive )
{
    // convert unsigned to signed
    DWORD msecs = umsecs;
    if(umsecs == MainLoop::WaitInfinite)
    {
        msecs = INFINITE;
    }
    else if( umsecs > std::numeric_limits<DWORD>::max() )
    {
        msecs = std::numeric_limits<DWORD>::max();
    }

    std::list<IOHandle*>::iterator iter;
    for( iter = _dirty.begin(); iter != _dirty.end(); ++iter )
    {
        // TODO: handle immediate avail by calling setAvail in Selectabe
        _handles.add( (*iter)->handle(), (*iter)->sel);
    }

    _dirty.clear();

    if( _avail.size() )
    {
        msecs = 0;
    }

    bool isTimeout = false;
    DWORD offset = waitFor(_handles.size(), _handles.handles(), msecs, isTimeout);

    try
    {
        // check all selectables that did not require waiting
        for( _currentAvail = _avail.begin(); _currentAvail != _avail.end(); )
        {
            Selectable* s = *_currentAvail;

            //if( s->enabled() ) 
                s->run();

            if( _currentAvail != _avail.end() &&
               *_currentAvail == s )
            {
                    ++_currentAvail;
            }
        }


        if(isTimeout)
            return;

        // wake event at offset 0 was active
        if (offset == 0)
        {
            isActive = EventDispatcher::processEvents();
            return;
        }
        // I/O event at offset 1 was active
        else if (offset == 1)
        {
            for( _current = _devices.begin(); _current != _devices.end(); )
            {
                Selectable* dev = *_current;

                //std::cerr << "ON AVAIL" << std::endl;
                //if( dev->enabled() ) 
                    dev->run();

                //std::cerr << "ITERATOR AFTER AVAIL" << std::endl;
                if( _current != _devices.end() && *_current == dev )
                {
                    ++_current;
                }
            }
        }
        else if (offset == 2)
        {
            for(;;)
            {
                Pt::System::MutexLock lock(_signalledMutex);
                if( _signalled.empty() )
                    break;
                
                Selectable* selectable = *(_signalled.begin());
                _signalled.erase( _signalled.begin() );
                lock.unlock();

                selectable->run();
            }
        }
        // some of the other event handles was active
        else if( offset < _handles.size() )
        {
            Selectable* selectable = _handles.at(offset);

            //if( selectable->enabled() )
                 selectable->run();
        }
    }
    catch (...)
    {
        _current = _devices.end();
        _currentAvail = _avail.end();
        throw;
    }
}


DWORD EventLoopImpl::waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout)
{
    DWORD result = WaitForMultipleObjects( numHandles, handles, false, msecs );
    if(result == WAIT_FAILED)
    {
        //DWORD err = GetLastError();
        throw IOError( PT_ERROR_MSG("WaitForMultipleObjects failed") );
    }

    if( result == WAIT_TIMEOUT)
    {
        isTimeout = true;
        return 0;
    }

    return result - WAIT_OBJECT_0;
}


MainLoopImpl::MainLoopImpl()
: EventLoopImpl()
{
//  _current = _devices.end();
//  _currentAvail = _avail.end();
//
//  _wakeEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
//  if( _wakeEvent == NULL )
//      throw SystemError( PT_ERROR_MSG("CreateEvent failed") );
//
//  _ioEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
//  if( _ioEvent == NULL )
//  {
//      CloseHandle( _wakeEvent );
//      throw SystemError( PT_ERROR_MSG("CreateEvent failed") );
//  }
//
//  _handles.add( _wakeEvent, 0 );
//  _handles.add( _ioEvent, 0 );
}

MainLoopImpl::MainLoopImpl(Allocator& a)
: EventLoopImpl()
{
//  _current = _devices.end();
//  _currentAvail = _avail.end();
//
//  _wakeEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
//  if( _wakeEvent == NULL )
//      throw SystemError( PT_ERROR_MSG("CreateEvent failed") );
//
//  _ioEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
//  if( _ioEvent == NULL )
//  {
//      CloseHandle( _wakeEvent );
//      throw SystemError( PT_ERROR_MSG("CreateEvent failed") );
//  }
//
//  _handles.add( _wakeEvent, 0 );
//  _handles.add( _ioEvent, 0 );
}


MainLoopImpl::~MainLoopImpl()
{
//  std::set<Selectable*>::iterator it;
//
//  while( _attached.size() )
//  {
//      it = _attached.begin();
//      (*it)->setParent(0);
//  }
//
//  CloseHandle( _wakeEvent );
//  CloseHandle( _ioEvent );
}


/*void MainLoopImpl::attach(Selectable& s)
{
    _attached.insert(&s);
}*/


/*void MainLoopImpl::detach(Selectable& s)
{
    _attached.erase(&s);
}*/


/*void MainLoopImpl::enable(Selectable& s)
{
    bool ready = false;
    bool accept = s.simpl().setWaitHandle(_ioEvent, ready);
    if(accept)
    {
        _devices.insert(&s);

        if(ready)
            _avail.insert(&s);
    }
    else
    {
        _dirty.insert(&s);
    }

    /// OLD:
    ///_dirty.insert(&s);
}*/


/*void MainLoopImpl::disable(Selectable& s)
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
}*/

/*void MainLoopImpl::idle(Selectable& s)
{
    std::set<Selectable*>::iterator it = _avail.find( &s );
    if( it == _avail.end() )
        return;

    if( _currentAvail != _avail.end() &&
       *_currentAvail == *it )
        _avail.erase(_currentAvail++);
    else
        _avail.erase(it);
}*/


/*void MainLoopImpl::active(Selectable& s)
{
    std::set<Selectable*>::iterator it = _avail.find( &s );
    if( it == _avail.end() )
        return;

    if( _currentAvail != _avail.end() &&
       *_currentAvail == *it )
        _avail.erase(_currentAvail++);
    else
        _avail.erase(it);
}*/


/*void MainLoopImpl::avail(Selectable& s)
{
    _avail.insert(&s);
}*/


/*void MainLoopImpl::changed(Selectable& s)
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
}*/


/*void MainLoopImpl::onRun()
{
    bool isActive = true;
    while(isActive)
    {
        size_t timeout = this->processTimers();

        this->waitNext(timeout, isActive);
    }
}*/


/*void MainLoopImpl::onWake()
{
    SetEvent( _wakeEvent );
}*/


/*void MainLoopImpl::waitNext(std::size_t umsecs, bool& isActive )
{
    // convert unsigned to signed
    DWORD msecs = umsecs;
    if(umsecs == MainLoop::WaitInfinite)
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

    //DWORD result = WaitForMultipleObjects( _handles.size(), _handles.handles(), false, msecs );
    //if(result == WAIT_FAILED)
    //{
    //    //DWORD err = GetLastError();
    //    throw IOError( PT_ERROR_MSG("WaitForMultipleObjects failed") );
    //}

    bool isTimeout = false;
    DWORD offset = waitFor(_handles.size(), _handles.handles(), msecs, isTimeout);

    try
    {
        // check all selectables that did not require waiting
        for( _currentAvail = _avail.begin(); _currentAvail != _avail.end(); )
        {
            Selectable* s = *_currentAvail;

            if( s->enabled() ) 
                s->simpl().checkEvent();

            if( _currentAvail != _avail.end() &&
               *_currentAvail == s )
            {
                    ++_currentAvail;
            }
        }

        //if( result == WAIT_TIMEOUT)
        //{
        //    return;
        //}

        if(isTimeout)
            return;

        //const Pt::ssize_t offset = (result - WAIT_OBJECT_0);

        // wake event at offset 0 was active
        if (offset == 0)
        {
            isActive = EventDispatcher::processEvents();
            return;
        }
        // I/O event at offset 1 was active
        else if (offset == 1)
        {
            for( _current = _devices.begin(); _current != _devices.end(); )
            {
                Selectable* dev = *_current;

                if( dev->enabled() ) 
                    dev->simpl().checkEvent();

                if( _current != _devices.end() &&
                   *_current == dev )
                {
                    ++_current;
                }
            }
        }
        // some of the other event handles was active
        else if( offset < _handles.size() )
        {
            Selectable* selectable = _handles.at(offset);

            if( selectable->enabled() )
                 selectable->simpl().checkEvent();
        }
    }
    catch (...)
    {
        _current = _devices.end();
        _currentAvail = _avail.end();
        throw;
    }
}*/


/*DWORD MainLoopImpl::waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout)
{
    DWORD result = WaitForMultipleObjects( numHandles, handles, false, msecs );
    if(result == WAIT_FAILED)
    {
        //DWORD err = GetLastError();
        throw IOError( PT_ERROR_MSG("WaitForMultipleObjects failed") );
    }

    if( result == WAIT_TIMEOUT)
    {
        isTimeout = true;
        return 0;
    }

    return result - WAIT_OBJECT_0;
}*/

} //namespace System

} //namespace Pt
