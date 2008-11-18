/***************************************************************************
 *   Copyright (C) 2005 - 2007 by Marc Boris D�rner                        *
 *   Copyright (C) 2005 - 2007 by Aloysius Indrayanto                      *
 *   Copyright (C) 2005 - 2007 by Sebastian Pieck                          *
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
#include "Pt/System/SystemError.h"
#include <sys/time.h>
#include <errno.h>

namespace Pt {

namespace System {

void MutexImpl::throw_if(bool flag, const char* msg, const Pt::SourceInfo& si)
{
   if(flag)
   {
        throw SystemError(msg, si);
   }
}


MutexImpl::MutexImpl()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK  );
    pthread_mutex_init(&_handle, &attr);
}


MutexImpl::MutexImpl(int recursive)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE );
    pthread_mutex_init(&_handle, &attr);
}


MutexImpl::~MutexImpl()
{
    pthread_mutex_destroy(&_handle);
}


void MutexImpl::lock()
{
   throw_if( pthread_mutex_lock(&_handle) != 0,
             "Could not lock mutex: ", PT_SOURCEINFO );
}


bool MutexImpl::tryLock()
{
    int ret = pthread_mutex_trylock(&_handle);
    if (ret == EBUSY)
        return false;

    throw_if(ret != 0, "Could not lock mutex", PT_SOURCEINFO);
    return true;
}


void MutexImpl::unlock()
{
   throw_if( pthread_mutex_unlock(&_handle) != 0, "Could not unlock mutex", PT_SOURCEINFO);
}


ReadWriteMutexImpl::ReadWriteMutexImpl()
{
    MutexImpl::throw_if( pthread_rwlock_init(&_rwl, NULL),
                         "Could not create rwlock", PT_SOURCEINFO );
}


ReadWriteMutexImpl::~ReadWriteMutexImpl()
{
    pthread_rwlock_destroy(&_rwl);
}


void ReadWriteMutexImpl::readLock()
{
    MutexImpl::throw_if( pthread_rwlock_rdlock(&_rwl) != 0,
                         "Could not lock rwlock", PT_SOURCEINFO );
}


bool ReadWriteMutexImpl::tryReadLock()
{
    int rc = pthread_rwlock_tryrdlock(&_rwl);
    if(rc == EBUSY)
        return false;

    MutexImpl::throw_if(rc != 0, "Could not lock rwlock", PT_SOURCEINFO);
    return true;
}


void ReadWriteMutexImpl::writeLock()
{
    MutexImpl::throw_if( pthread_rwlock_wrlock(&_rwl) != 0,
                         "Could not lock rwlock", PT_SOURCEINFO );
}


bool ReadWriteMutexImpl::tryWriteLock()
{
    int rc = pthread_rwlock_trywrlock(&_rwl);
    if(rc == EBUSY)
        return false;

    MutexImpl::throw_if(rc != 0, "Could not lock rwlock", PT_SOURCEINFO);
    return true;
}


void ReadWriteMutexImpl::unlock()
{
    MutexImpl::throw_if( pthread_rwlock_unlock(&_rwl) != 0,
                         "Could not unlock mutex", PT_SOURCEINFO );
}

} // !namepsace System

} // !namespace Pt
