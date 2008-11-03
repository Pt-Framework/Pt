/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *                                                                         *
 ***************************************************************************/
#include "ThreadImpl.h"
#include "Pt/Types.h"
#include "Pt/System/SystemError.h"

namespace Pt {

namespace System {

ThreadImpl::~ThreadImpl()
{
    this->close();
	delete _cb;
}


void ThreadImpl::init(const Callable<void>& cb)
{
	if(_cb)
	{
		delete _cb;
		_cb = cb.clone();
	}
}

void ThreadImpl::close()
{
    if (_handle != 0) 
	{
        ::CloseHandle(_handle);
        _handle = 0;
    }
}


void ThreadImpl::start() 
{
    SIZE_T stackSize = 0;

#ifdef _WIN32_WCE
    _handle = ::CreateThread(NULL, stackSize, entry, this, 0, &_id);
#else
    _handle = (HANDLE)_beginthreadex(NULL, stackSize, entry, this, 0, &_id);
#endif

    if(_handle == NULL) 
	{
        _id = 0;
        throw SystemError("Thread creation failed", PT_SOURCEINFO);
    }
}


void ThreadImpl::join()
{
    DWORD status = ::WaitForSingleObject(_handle, INFINITE);
    if( status != WAIT_OBJECT_0 )
        throw SystemError("Could not join thread", PT_SOURCEINFO);

    _id = 0;
}


void ThreadImpl::terminate()
{
    if( ! TerminateThread(_handle, 0) )
        throw SystemError("Could not kill thread.", PT_SOURCEINFO);

    _id = 0;
}


void ThreadImpl::exit()
{
    DWORD status = 0;
    
#ifdef _WIN32_WCE
    ::ExitThread(status);
#else
    _endthreadex(status);
#endif
}


void ThreadImpl::sleep(unsigned int ms)
{
#ifdef _WIN32_WCE
	::Sleep(ms);
#else
	::SleepEx(ms, FALSE);
#endif
}

} // namespace System

} // namespace Pt
