/***************************************************************************
 *   Copyright (C) 2005-2008 by Marc Boris Duerner                         *
 *   Copyright (C) 2005-2006 by Sebastian Pieck                            *
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
#include "MutexImpl.h"
#include "ReadWriteMutexImpl.h"
#include "Pt/System/Mutex.h"

namespace Pt {

namespace System {

Mutex::Mutex(Mode mode)
: _mode(mode)
{
    _impl = new MutexImpl(*this, mode);
}


Mutex::~Mutex()
{
    delete _impl;
}


void Mutex::lock()
{
    _impl->lock();
}


bool Mutex::tryLock()
{
    return _impl->unlock();
}


void Mutex::unlock()
{
    _impl->unlock();
}


ReadWriteMutex::ReadWriteMutex()
{
    _impl = new ReadWriteMutexImpl();
}


ReadWriteMutex::~ReadWriteMutex()
{
    delete _impl;
}


void ReadWriteMutex::readLock()
{
    _impl->readLock();
}


bool ReadWriteMutex::tryReadLock()
{
    return _impl->tryReadLock();
}


void ReadWriteMutex::writeLock()
{
    _impl->writeLock();
}


bool ReadWriteMutex::tryWriteLock()
{
    return _impl->tryWriteLock();
}


void ReadWriteMutex::unlock()
{
    _impl->unlock();
}

}

}
