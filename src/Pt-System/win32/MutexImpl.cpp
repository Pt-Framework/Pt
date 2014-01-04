/*
 * Copyright (C) 2006- 2013 Marc Boris Duerner
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
#include "MutexImpl.h"
#include "Pt/SourceInfo.h"
#include "Pt/System/SystemError.h"

namespace Pt {

namespace System {

MutexImpl::MutexImpl()
{
#ifdef __cplusplus_winrt
	InitializeCriticalSectionEx(&_handle, 1, 0);
#else
	InitializeCriticalSection(&_handle);
#endif
}


MutexImpl::MutexImpl(int)
{
#ifdef __cplusplus_winrt
	InitializeCriticalSectionEx(&_handle, 1, 0);
#else
	InitializeCriticalSection(&_handle);
#endif
}


MutexImpl::~MutexImpl()
{
	DeleteCriticalSection(&_handle);
}


void MutexImpl::lock()
{    
	EnterCriticalSection(&_handle);
}


bool MutexImpl::tryLock()
{
	DWORD ret = TryEnterCriticalSection(&_handle);
    return ret != 0;
}


void MutexImpl::unlock()
{
	LeaveCriticalSection(&_handle);
}


ReadWriteMutexImpl::ReadWriteMutexImpl()
: _readers(0), _writers(0)
{
#ifdef __cplusplus_winrt
    _mutex = CreateMutexExW(NULL, NULL, 0, SYNCHRONIZE);
#else
	  _mutex = CreateMutex(NULL, FALSE, NULL);
#endif

	  if(_mutex == NULL)
		    throw SystemError( PT_ERROR_MSG("CreateMutex failed") );

#ifdef __cplusplus_winrt
    _readEvent = CreateEventExW(NULL, NULL, CREATE_EVENT_MANUAL_RESET|CREATE_EVENT_INITIAL_SET, SYNCHRONIZE);
#else
	  _readEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
#endif
	  
    if(_readEvent == NULL)
		    throw SystemError( PT_ERROR_MSG("CreateEvent failed") );

#ifdef __cplusplus_winrt
    _writeEvent = CreateEventExW(NULL, NULL, CREATE_EVENT_MANUAL_RESET|CREATE_EVENT_INITIAL_SET, SYNCHRONIZE);
#else
	  _writeEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
#endif

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

#ifdef __cplusplus_winrt
    DWORD ret = WaitForMultipleObjectsEx(2, h, TRUE, INFINITE, FALSE);
#else
    DWORD ret = WaitForMultipleObjects(2, h, TRUE, INFINITE);
#endif

	  switch(ret)
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

#ifdef __cplusplus_winrt
    DWORD ret = WaitForMultipleObjectsEx(2, h, TRUE, 1, FALSE);
#else
    DWORD ret = WaitForMultipleObjects(2, h, TRUE, 1);
#endif

	  switch(ret)
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

#ifdef __cplusplus_winrt
    DWORD ret = WaitForMultipleObjectsEx(2, h, TRUE, INFINITE, FALSE);
#else
    DWORD ret = WaitForMultipleObjects(2, h, TRUE, INFINITE);
#endif

	  switch(ret)
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

#ifdef __cplusplus_winrt
    DWORD ret = WaitForMultipleObjectsEx(2, h, TRUE, 1, FALSE);
#else
    DWORD ret = WaitForMultipleObjects(2, h, TRUE, 1);
#endif

	  switch(ret)
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

#ifdef __cplusplus_winrt
    DWORD ret = WaitForSingleObjectEx(_mutex, INFINITE, FALSE);
#else
    DWORD ret = WaitForSingleObject(_mutex, INFINITE);
#endif

	  switch(ret)
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
#ifdef __cplusplus_winrt
    DWORD ret = WaitForSingleObjectEx(_mutex, INFINITE, FALSE);
#else
    DWORD ret = WaitForSingleObject(_mutex, INFINITE);
#endif

	  switch (ret)
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
#ifdef __cplusplus_winrt
    DWORD ret = WaitForSingleObjectEx(_mutex, INFINITE, FALSE);
#else
    DWORD ret = WaitForSingleObject(_mutex, INFINITE);
#endif

	  switch( ret )
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
