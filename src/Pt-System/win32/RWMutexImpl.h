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
#include <windows.h>
#include "Pt/System/SystemError.h"

namespace Pt {

namespace System {


class RWMutexImpl
{
    public:
        RWMutexImpl()
        : _readers(0), _writers(0)
        {
            _mutex = CreateMutex(NULL, FALSE, NULL);

            if(_mutex == NULL)
                throw SystemError("Could not create reader/writer lock", PT_SOURCEINFO);

            _readEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
            if(_readEvent == NULL)
                throw SystemError("Could not create reader/writer lock", PT_SOURCEINFO);

            _writeEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
            if(_writeEvent == NULL)
                throw SystemError("Could not create reader/writer lock", PT_SOURCEINFO);
        }

        ~RWMutexImpl()
        {
            CloseHandle(_mutex);
            CloseHandle(_readEvent);
            CloseHandle(_writeEvent);
        }

        void readLock()
        {
            HANDLE h[2];
            h[0] = _mutex;
            h[1] = _readEvent;

            switch( WaitForMultipleObjects(2, h, TRUE, INFINITE) )
            {
                case WAIT_OBJECT_0:
                case WAIT_OBJECT_0 + 1:
                    ++_readers;
                    ResetEvent(_writeEvent);
                    ReleaseMutex(_mutex);
                    break;
                default:
                    throw SystemError("Could not aquire reader lock", PT_SOURCEINFO);
            }
        }

        bool tryReadLock()
        {
            HANDLE h[2];
            h[0] = _mutex;
            h[1] = _readEvent;

            switch( WaitForMultipleObjects(2, h, TRUE, 1) )
            {
                case WAIT_OBJECT_0:
                case WAIT_OBJECT_0 + 1:
                    ++_readers;
                    ResetEvent(_writeEvent);
                    ReleaseMutex(_mutex);
                    return true;
                case WAIT_TIMEOUT:
                    return false;
                default:
                    throw SystemError("Could not aquire reader lock", PT_SOURCEINFO);
            }
        }

        void writeLock()
        {
            this->addWriter();

            HANDLE h[2];
            h[0] = _mutex;
            h[1] = _writeEvent;

            switch( WaitForMultipleObjects(2, h, TRUE, INFINITE) )
            {
                case WAIT_OBJECT_0:
                case WAIT_OBJECT_0 + 1:
                    --_writers;
                    ++_readers;
                    ResetEvent(_readEvent);
                    ResetEvent(_writeEvent);
                    ReleaseMutex(_mutex);
                    break;
                default:
                    this->removeWriter();
                    throw SystemError("Could not aquire writer lock", PT_SOURCEINFO);
            }
        }

        bool tryWriteLock()
        {
            this->addWriter();

            HANDLE h[2];
            h[0] = _mutex;
            h[1] = _writeEvent;

            switch (WaitForMultipleObjects(2, h, TRUE, 1))
            {
                case WAIT_OBJECT_0:
                case WAIT_OBJECT_0 + 1:
                    --_writers;
                    ++_readers;
                    ResetEvent(_readEvent);
                    ResetEvent(_writeEvent);
                    ReleaseMutex(_mutex);
                    return true;
                case WAIT_TIMEOUT:
                    this->removeWriter();
                    return false;
                default:
                    removeWriter();
                    throw SystemError("Could not aquire writer lock", PT_SOURCEINFO);
            }

        }

        void unlock()
        {
            switch (WaitForSingleObject(_mutex, INFINITE))
            {
                case WAIT_OBJECT_0:
                    if (_writers == 0) SetEvent(_readEvent);
                    if (--_readers == 0) SetEvent(_writeEvent);
                    ReleaseMutex(_mutex);
                    break;
                default:
                    throw SystemError("Could not lock reader/writer lock", PT_SOURCEINFO);
            }
        }

    private:
        void addWriter()
        {
            switch ( WaitForSingleObject(_mutex, INFINITE) )
            {
                case WAIT_OBJECT_0:
                    if (++_writers == 1) ResetEvent(_readEvent);
                    ReleaseMutex(_mutex);
                    break;
                default:
                    throw SystemError("Could not lock reader/writer lock", PT_SOURCEINFO);
            }
        }


        void removeWriter()
        {
            switch( WaitForSingleObject(_mutex, INFINITE) )
            {
                case WAIT_OBJECT_0:
                    if (--_writers == 0) SetEvent(_readEvent);
                    ReleaseMutex(_mutex);
                    break;
                default:
                    throw SystemError("Could not lock reader/writer lock", PT_SOURCEINFO);
            }
        }

        HANDLE   _mutex;
        HANDLE   _readEvent;
        HANDLE   _writeEvent;
        unsigned _readers;
        unsigned _writers;
};

} // namespace System

} // namespace Pt
