/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *                                                                         *
 ***************************************************************************/

#include "ThreadImpl.h"

#include "Pt/Types.h"
#include "Pt/System/SystemError.h"

#include <sstream>

namespace Pt {

namespace System {


ThreadImpl::ThreadImpl(Thread& obj, Thread::Mode mode)
: _thread(obj),
  _handle(0),
  _id(0),
  _priority(InheritPriority),
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
    if (_handle != 0) {
        ::CloseHandle(_handle);
        _handle = 0;
    }
}


void ThreadImpl::setPriority(Priority prio)
{
    _priority = prio;

    // only save new priority if thread is not running
    if(_state != Thread::Running) {
        return;
    }

    int winPrio = THREAD_PRIORITY_NORMAL;
    switch(_priority) {
        case LowestPriority:
            winPrio = THREAD_PRIORITY_LOWEST;
            break;
        case LowPriority:
            winPrio = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        case NormalPriority:
            winPrio = THREAD_PRIORITY_NORMAL;
            break;
        case HighPriority:
            winPrio = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case HighestPriority:
            winPrio = THREAD_PRIORITY_HIGHEST;
            break;
            
        case InheritPriority:
            // TODO: InheritPriority ???
            break;
    }

    if(0 == ::SetThreadPriority(_handle, winPrio) )
        throw SystemError("Could not set priority.", PT_SOURCEINFO);
}


void ThreadImpl::start(Thread::Mode mode) 
{
    SIZE_T stackSize = 0;

#ifdef _WIN32_WCE
    _handle = ::CreateThread(NULL, stackSize, entry, this, 0, &_id);
#else
    _handle = (HANDLE)_beginthreadex(NULL, stackSize, entry, this, 0, &_id);
#endif

    if(_handle == NULL) {
        _id = 0;
        Pt::uint32_t errorCode = GetLastError();
        std::stringstream ss;
        ss << "Could not create thread. System error code: " << errorCode;
        throw SystemError(ss.str(), PT_SOURCEINFO);
    }

    _state = Thread::Running;

    // setPriority() might have been called before start()
    if(_priority != InheritPriority) {
        this->setPriority(_priority);
    }

    if(_mode == Thread::Detached) {
        this->close();
    }
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


void ThreadImpl::exit() throw()
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
    if( !TerminateThread(_handle, 0) )
        throw SystemError("Could not kill thread.", PT_SOURCEINFO);

    _state = Thread::Finished;
    _id = 0;
}


void ThreadImpl::yield() throw()
{
#ifdef _WIN32_WCE
    ::Sleep(0);
#else
    ::SleepEx(0, FALSE);
#endif
}


void ThreadImpl::sleep(unsigned int ms) throw()
{
#ifdef _WIN32_WCE
    ::Sleep(ms);
#else
    ::SleepEx(ms, FALSE);
#endif
}


} // namespace System

} // namespace Pt









