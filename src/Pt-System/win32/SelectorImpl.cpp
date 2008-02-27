/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Bjoern Oliver Streule                         *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
#include "IODeviceImpl.h"
#include "Pt/System/Selector.h"
#include <algorithm>

namespace Pt {

namespace System {

SelectorImpl::SelectorImpl()
{
    _readers.push_back( &_wakeResult );
}


SelectorImpl::~SelectorImpl()
{
    std::vector<IOResult*>::iterator iter;
    for( iter = _readers.begin(); iter != _readers.end(); ++iter )
    {
        IOResult* result = *iter;
        result->setSelector(0);
    }
}


void SelectorImpl::complete( IOResult& result )
{
    //TODO: check if result exists.
    _readers.push_back(&result);
}


void SelectorImpl::cancel( IOResult& result )
{
    std::vector<IOResult*>::iterator it;
    for (it = _readers.begin() ; it != _readers.end(); ++it)
    {
        if (*it == &result)
        {
            (*it)->setSelector(0);
            it = _readers.erase(it);
            return;
        }
    }
}


bool SelectorImpl::wait( unsigned int msecs )
{
    std::vector<HANDLE> waitHandles;

    if( msecs == Selector::WaitInfinite ) {
        msecs = INFINITE;
    }

    std::vector<IOResult*>::iterator iter;
    for( iter = _readers.begin(); iter != _readers.end(); ++iter )
    {
        IOResult* result = *iter;
        HANDLE handle = result->impl()->handle();
        waitHandles.push_back( handle );
    }

    DWORD result = WaitForMultipleObjects( waitHandles.size(), &waitHandles[0], false, msecs );

    if( result == WAIT_TIMEOUT ) {
        return false;
    }

    const Pt::ssize_t handleIndex  = (result - WAIT_OBJECT_0);

    IOResult* res = _readers[handleIndex];
    if (res != static_cast<IOResult*>(&_wakeResult) )
    {
        std::vector<IOResult*>::iterator done = _readers.begin() + handleIndex;
        (*done)->setSelector(0);
        _readers.erase(done);
    }
    res->impl()->onComplete();

    return true;
}


void SelectorImpl::wake()
{
    _wakeResult.wake();
}

} //namespace System

} //namespace Pt
