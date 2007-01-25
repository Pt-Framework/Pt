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

#include <pthread.h>
#include <errno.h>

#include "Pt/System/SystemError.h"


namespace Pt {

namespace System {


class RWLockImpl
{
    public:
        RWLockImpl()
        {
            if( pthread_rwlock_init(&_rwl, NULL) )
                throw SystemError("Could not create reader/writer lock", PT_SOURCEINFO);
        }

        ~RWLockImpl()
        {
            pthread_rwlock_destroy(&_rwl);
        }

        void readLock()
        {
            if( pthread_rwlock_rdlock(&_rwl) )
                throw SystemError("Could not lock reader/writer lock", PT_SOURCEINFO);
        }

        bool tryReadLock()
        {
            int rc = pthread_rwlock_tryrdlock(&_rwl);
            if (rc == 0)
                return true;
            else if (rc == EBUSY)
                return false;
            else
                throw SystemError("Could not lock reader/writer lock", PT_SOURCEINFO);

        }

        void writeLock()
        {
            if( pthread_rwlock_wrlock(&_rwl) )
                throw SystemError("Could not lock reader/writer lock", PT_SOURCEINFO);
        }

        bool tryWriteLock()
        {
            int rc = pthread_rwlock_trywrlock(&_rwl);
            if(rc == 0)
                return true;
            else if (rc == EBUSY)
                return false;
            else
                throw SystemError("Could not lock reader/writer lock", PT_SOURCEINFO);

        }

        void unlock()
        {
            if( pthread_rwlock_unlock(&_rwl) )
                throw SystemError("Could not unlock mutex", PT_SOURCEINFO);
        }

    private:
        pthread_rwlock_t _rwl;
};

} // namespace System

} // namespace Pt
