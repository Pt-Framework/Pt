/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *                                                                         *
 ***************************************************************************/

#include "ThreadImpl.h"
#include "Pt/Types.h"
#include "Pt/System/SystemError.h"

namespace Pt {

namespace System {

ThreadImpl::ThreadImpl(Thread& obj, Thread::Mode mode)
: _thread(obj),
  _handle(0),
  _id(0),
  _state(Thread::Ready),
  _mode(mode)
{
}


ThreadImpl::~ThreadImpl()
{
    this->close();
}


void ThreadImpl::close()
{
    if (_handle != 0) 
	{
        ::CloseHandle(_handle);
        _handle = 0;
    }
}


void ThreadImpl::start(Thread::Mode mode) 
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

    _state = Thread::Running;

    if(_mode == Thread::Detached)
        this->close();
}


void ThreadImpl::detach()
{
    // simply close the thread control handle.
    this->close();
    _mode = Thread::Detached;
}


void ThreadImpl::wait()
{
    DWORD status = ::WaitForSingleObject(_handle, INFINITE);
    if( status != WAIT_OBJECT_0 )
        throw SystemError("Could not join thread", PT_SOURCEINFO);

    _state = Thread::Finished;
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


void ThreadImpl::terminate()
{
    if( ! TerminateThread(_handle, 0) )
        throw SystemError("Could not kill thread.", PT_SOURCEINFO);

    _state = Thread::Finished;
    _id = 0;
}


void ThreadImpl::yield()
{
#ifdef _WIN32_WCE
    ::Sleep(0);
#else
    ::SleepEx(0, FALSE);
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
