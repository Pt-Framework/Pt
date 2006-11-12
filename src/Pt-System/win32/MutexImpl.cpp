/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "MutexImpl.h"

#include "Pt/Sourceinfo.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/Mutex.h"


namespace Pt {

namespace System {


MutexImpl::MutexImpl(Mutex& mutex)
: _mutex(mutex)
{
	_handle = CreateMutex(NULL, FALSE, NULL);

	if( !_handle )
		throw SystemError("Could not create mutex: ", PT_SOURCEINFO);
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
		throw SystemError ("Could not wait for mutex: ", PT_SOURCEINFO);
}


bool MutexImpl::tryLock(unsigned int msec)
{
	#ifdef _WIN32_WCE
		DWORD ret = WaitForSingleObject(_handle, msec);
	#else
		DWORD ret = WaitForSingleObjectEx(_handle, msec, FALSE);
	#endif
	
	if(ret == WAIT_FAILED) {
		throw SystemError ("Could not wait for mutex: ", PT_SOURCEINFO);
	}
	else if(ret == WAIT_OBJECT_0)
		return true;

	return false;
}


void MutexImpl::unlock()
{
	if( !ReleaseMutex(_handle) )
		throw SystemError("Could not release mutex: ", PT_SOURCEINFO);
}


} // namespace System

} // namespace Pt
