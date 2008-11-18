/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Sebastian Pieck                               *
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
#include "MutexImpl.h"
#include "Pt/SourceInfo.h"
#include "Pt/System/SystemError.h"

namespace Pt {

namespace System {

MutexImpl::MutexImpl()
{
    _handle = CreateMutex(NULL, FALSE, NULL);

    if( !_handle )
        throw SystemError("Could not create mutex", PT_SOURCEINFO);
}


MutexImpl::MutexImpl(int recursive)
{
    _handle = CreateMutex(NULL, FALSE, NULL);

    if( !_handle )
        throw SystemError("Could not create mutex", PT_SOURCEINFO);
}


MutexImpl::~MutexImpl()
{
    ::CloseHandle(_handle);
}


void MutexImpl::lock()
{
    #ifdef _WIN32_WCE
        DWORD ret = WaitForSingleObject(_handle, INFINITE);
    #else
        DWORD ret = WaitForSingleObjectEx(_handle, INFINITE, FALSE);
    #endif

    if(ret != WAIT_OBJECT_0)
    {
        DWORD error =  GetLastError();
        throw SystemError ("Could not wait for mutex", PT_SOURCEINFO);
    }
}


bool MutexImpl::tryLock()
{
    #ifdef _WIN32_WCE
        DWORD ret = WaitForSingleObject(_handle, 0);
    #else
        DWORD ret = WaitForSingleObjectEx(_handle, 0, FALSE);
    #endif

    if(ret == WAIT_OBJECT_0)
        return true;

    if(ret == WAIT_TIMEOUT)
        return false;

    throw SystemError ("Could not wait for mutex", PT_SOURCEINFO);
    return false;
}


void MutexImpl::unlock()
{
    if( ! ReleaseMutex(_handle) )
    {
        DWORD error =  GetLastError();
        throw SystemError("Could not release mutex", PT_SOURCEINFO);
    }
}


ReadWriteMutexImpl::ReadWriteMutexImpl()
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


ReadWriteMutexImpl::~ReadWriteMutexImpl()
{
	CloseHandle(_mutex);
	CloseHandle(_readEvent);
	CloseHandle(_writeEvent);
}


void ReadWriteMutexImpl::readLock()
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


bool ReadWriteMutexImpl::tryReadLock()
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


void ReadWriteMutexImpl::writeLock()
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


bool ReadWriteMutexImpl::tryWriteLock()
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


void ReadWriteMutexImpl::unlock()
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


void ReadWriteMutexImpl::addWriter()
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


void ReadWriteMutexImpl::removeWriter()
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

} // namespace System

} // namespace Pt
