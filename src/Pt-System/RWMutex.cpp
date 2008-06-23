/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
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
#include "Pt/System/RWMutex.h"
#include "RWMutexImpl.h"

namespace Pt {

namespace System {

RWMutex::RWMutex()
{
    _impl = new RWMutexImpl();
}


RWMutex::~RWMutex()
{
    delete _impl;
}


void RWMutex::readLock()
{
    _impl->readLock();
}


bool RWMutex::tryReadLock()
{
    return _impl->tryReadLock();
}


void RWMutex::writeLock()
{
    _impl->writeLock();
}


bool RWMutex::tryWriteLock()
{
    return _impl->tryWriteLock();
}


void RWMutex::unlock()
{
    _impl->unlock();
}

}

}
