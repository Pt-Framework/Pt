/*
 * Copyright (C) 2008 Peter Barth
 * Copyright (C) 2008 PTV AG
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

#include "SymbEventLoop.h"
#include <iostream>
#include "ApplicationImpl.h"

#include <Pt/System/MutexLock.h>

#include <basched.h>

CSymbEventLoop* CSymbEventLoop::NewL(Pt::Gui::ApplicationImpl& appImpl)
{
    CSymbEventLoop* self = 0;
    
    // The constructor could actually throw an exception derived from
    // std::exception due to Pt attributes being initialized.
    // If this is the case we catch it and convert it into a leave.
    // On Symbian 9.x a leave itself is based on the c++ exception mechanism 
    // but the exception thrown is not a standard c++ exception.
    // (e.g. std::bad_alloc or std::exception).
    // Take a look at the definition of TRAP/TRAPD
    try
    {
        self = new (ELeave) CSymbEventLoop(appImpl);
    }
    catch (std::exception& e)
    {
        User::Leave(KErrGeneral);
    }
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CSymbEventLoop::CSymbEventLoop(Pt::Gui::ApplicationImpl& appImpl)
: CActive(EPriorityIdle)
, _appImpl(appImpl)
, _running(false)
, _runningMutex(Pt::System::Mutex::Normal)
, _mutex(Pt::System::Mutex::Normal)
, _queueMutex(Pt::System::Mutex::Normal)
, _processMutex(Pt::System::Mutex::Normal)
, _wakeMutex(Pt::System::Mutex::Normal)
{
}

void CSymbEventLoop::ConstructL()
{
    _mainThreadId = RThread().Id();
    
    bool res = false;
    
    // in case the Pt primitives throw any exception take care of them
    // by converting them into an error code
    try
    {
        res = CreateThread();
    }
    catch (std::exception& e)
    {
        res = false;
    }
    
    if (!res)
    {
        User::Leave(KErrGeneral);
    }
}

CSymbEventLoop::~CSymbEventLoop()
{
    Stop();
    DrainQueue();
}

void CSymbEventLoop::RunL()
{
    if (iStatus.Int() == KErrNone)
    {
        if (ProcessEvents())
        {
            WaitForEvents();
        }
    }
}

TInt CSymbEventLoop::RunError(TInt err)
{
    if (err == KLeaveExit) //-1003
    {
        return err;
    }
    //handle other error codes
    return KErrNone;    
}

void CSymbEventLoop::DoCancel()
{
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrCancel);
}

void CSymbEventLoop::WaitForEvents()
{
    if (IsRunning())
    {
        _mutex.lock();
        iStatus = KRequestPending;
        SetActive();        
        _cond.signal();
        _mutex.unlock();
    }
}

bool CSymbEventLoop::CreateThread()
{
    int rc = ::pthread_create(&_thread, NULL, EventLoopThreadEntry, 
            reinterpret_cast<void*>(this));
    
    if (!rc)
    {
        _mutex.lock();
        _startCond.wait(_mutex);
        _mutex.unlock();
        return true;
    }

    return false;    
}

bool CSymbEventLoop::Start()
{
    CActiveScheduler::Add(this);
    
    return ProcessQueuedEvents();
}

bool CSymbEventLoop::Stop()
{
    if (!IsRunning())
        return false;
    
    SetRunning(false);

    Wake();
    
    Deque();
    
    _cond.signal();
    
    int rc = ::pthread_join(_thread, NULL);

    return rc == 0;
}

void CSymbEventLoop::CommitEvent(const Pt::Event& event)
{
    TThreadId id = RThread().Id();
    
    // same thread?
    if (_mainThreadId == id)
    {
        _queueMutex.lock();
        Pt::Event* ev = event.clone();
        _eventQueue.push_back(ev);
        _queueMutex.unlock();
        this->Wake();
    }
    else
    {
        Pt::System::MutexLock lock(_processMutex);
        _queueMutex.lock();
        Pt::Event* ev = event.clone();
        _eventQueue.push_back(ev);
        _queueMutex.unlock();
        this->Wake();
    }    
}

void CSymbEventLoop::QueueEvent(const Pt::Event& event)
{
    TThreadId id = RThread().Id();
    
    // same thread?
    if (_mainThreadId == id)
    {
        Pt::System::MutexLock lock( _queueMutex );
        Pt::Event* ev = event.clone();
        _eventQueue.push_back(ev);
    }
    else
    {
        Pt::System::MutexLock lock1( _processMutex );
        Pt::System::MutexLock lock2( _queueMutex );
        Pt::Event* ev = event.clone();
        _eventQueue.push_back(ev);
    }
}

void CSymbEventLoop::Wake()
{
    // TODO: If selector is used to wait for events
    // use a mutex to protect it
    // Currently this is unused
    //Pt::System::MutexLock lock(_queueMutex);    
    //_selector.wake();

    _wakeCondition.signal();
}

bool CSymbEventLoop::ProcessQueuedEvents()
{
    Pt::System::MutexLock lock(_processMutex);
    
    while (true)
    {
        _queueMutex.lock();

        if (_eventQueue.empty())
        {
            _queueMutex.unlock();
            break;
        }

        Pt::Event* ev = _eventQueue.front();
        _eventQueue.remove(ev);

        _queueMutex.unlock();
        
        // if this returns false it means the we just processed an exit event
        if (!DispatchEvent(ev))
        {
            delete ev;
            return false;
        }

        delete ev;
    }    
    
    return true;
}

bool CSymbEventLoop::ProcessEvents()
{
    Pt::System::MutexLock lock(_processMutex);

    while (IsRunning())
    {
        _queueMutex.lock();

        if (_eventQueue.empty())
        {
            _queueMutex.unlock();
            break;
        }

        Pt::Event* ev = _eventQueue.front();
        _eventQueue.remove(ev);

        _queueMutex.unlock();

        // if this returns false it means the we just processed an exit event
        if (!DispatchEvent(ev))
        {
            delete ev;
            return false;
        }
        
        // note that when the above function fails
        // it it responsible for deleting ev before leaving
        delete ev;
    }
    
    return true;
}

void CSymbEventLoop::DrainQueue()
{
    Pt::System::MutexLock lock(_queueMutex);

    while (true)
    {
        if (_eventQueue.empty())
        {
            break;
        }

        Pt::Event* ev = _eventQueue.front();
        _eventQueue.remove(ev);

        delete ev;
    }    
}

bool CSymbEventLoop::DispatchEvent(Pt::Event* event)
{
    // If we're having an exit event we simply quit the application
    if (event->typeInfo() == typeid(Pt::Gui::ExitEvent)) 
    {
        Pt::Gui::Environment::instance().stopWaitLoop();    
        return false;
    }    
    
    Pt::Gui::Environment::instance().dispatchEvent(*event);
    return true;
}

void* CSymbEventLoop::EventLoopThreadEntry(void* threadID)
{
    CSymbEventLoop* self = 
        reinterpret_cast<CSymbEventLoop*>(threadID);

    self->EventLoopThread();

    ::pthread_exit(0);

    return 0;
}

void CSymbEventLoop::EventLoopThread()
{   
    RThread thread;
    thread.Open(_mainThreadId);
    
    SetRunning(true);
    
    _startCond.signal();

    while (IsRunning())
    {
        _mutex.lock();
        _cond.wait(_mutex);
        
        _queueMutex.lock();

        if (_eventQueue.empty() && IsRunning())
        {
            _queueMutex.unlock();

            _wakeMutex.lock();
            _wakeCondition.wait(_wakeMutex);
            _wakeMutex.unlock();
            //_selector.wait(Pt::System::Selector::WaitInfinite);
        }
        else
        {
            _queueMutex.unlock();
        }
        

        TRequestStatus* status = &iStatus;
        if (*status == KRequestPending)
        {
            thread.RequestComplete(status, 
                    IsRunning() ? KErrNone : KErrCompletion);
        }            

        _mutex.unlock();    
    }

    SetRunning(false);
    
    thread.Close();
}

void CSymbEventLoop::SetRunning(bool running)
{
    Pt::System::MutexLock lock(_runningMutex);
    _running = running;
}

bool CSymbEventLoop::IsRunning()
{
    Pt::System::MutexLock lock(_runningMutex);
    return _running;
}
