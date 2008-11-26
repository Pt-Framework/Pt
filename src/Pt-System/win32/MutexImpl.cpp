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
        throw SystemError( PT_ERROR_MSG("CreateMutex failed") );
}


MutexImpl::MutexImpl(int recursive)
{
    _handle = CreateMutex(NULL, FALSE, NULL);

    if( !_handle )
        throw SystemError( PT_ERROR_MSG("CreateMutex failed") );
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
        throw SystemError( PT_ERROR_MSG("WaitForSingleObject failed") );
}


bool MutexImpl::tryLock()
{
    #ifdef _WIN32_WCE
        DWORD ret = WaitForSingleObject(_handle, 0);
    #else
        DWORD ret = WaitForSingleObjectEx(_handle, 0, FALSE);
    #endif

    if(ret != WAIT_OBJECT_0 && ret != WAIT_TIMEOUT)
        throw SystemError( PT_ERROR_MSG("WaitForSingleObject failed") );

    return ret == WAIT_OBJECT_0;
}


void MutexImpl::unlock()
{
    if( ! ReleaseMutex(_handle) )
    {
        throw SystemError( PT_ERROR_MSG("ReleaseMutex failed") );
    }
}


ReadWriteMutexImpl::ReadWriteMutexImpl()
: _readers(0), _writers(0)
{
	_mutex = CreateMutex(NULL, FALSE, NULL);

	if(_mutex == NULL)
		throw SystemError( PT_ERROR_MSG("CreateMutex failed") );

	_readEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if(_readEvent == NULL)
		throw SystemError( PT_ERROR_MSG("CreateEvent failed") );

	_writeEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if(_writeEvent == NULL)
		throw SystemError( PT_ERROR_MSG("CreateEvent failed") );
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
			throw SystemError( PT_ERROR_MSG("WaitForMultipleObjects failed") );
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
			throw SystemError( PT_ERROR_MSG("WaitForMultipleObjects failed") );
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
			throw SystemError( PT_ERROR_MSG("WaitForMultipleObjects failed") );
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
			throw SystemError( PT_ERROR_MSG("WaitForMultipleObjects failed") );
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
			throw SystemError( PT_ERROR_MSG("WaitForSingleObject failed") );
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
			throw SystemError( PT_ERROR_MSG("WaitForSingleObject failed") );
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
			throw SystemError( PT_ERROR_MSG("WaitForSingleObject failed") );
	}
}

} // namespace System

} // namespace Pt
