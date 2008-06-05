/***************************************************************************
 *   Copyright (C) 2008 Peter Barth                                        *
 *   Copyright (C) 2008 PTV AG                                             *
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

#include "SymbEventLoop.h"
#include "ApplicationImpl.h"

#include <iostream>

#include <Pt/System/MutexLock.h>

SymbEventLoop* SymbEventLoop::NewL(Pt::Gui::ApplicationImpl& appImpl)
{
    SymbEventLoop* self = new (ELeave) SymbEventLoop(appImpl);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

SymbEventLoop::SymbEventLoop(Pt::Gui::ApplicationImpl& appImpl)
: CActive(EPriorityIdle)
, _appImpl(appImpl)
, _running(false)
, _queueMutex(Pt::System::Mutex::Normal)
//, _wakeMutex(Pt::System::Mutex::Normal)
, _processMutex(Pt::System::Mutex::Normal)
{
}

void SymbEventLoop::ConstructL()
{
    RThread thread;
    _mainThreadId = thread.Id();
    thread.Close();
    
    ::pthread_mutex_init(&_mutex, NULL);
    ::pthread_cond_init(&_cond, NULL);
    ::pthread_cond_init(&_startCond, NULL);     

    CreateThread();
}

SymbEventLoop::~SymbEventLoop()
{
    Stop();

    ::pthread_cond_destroy(&_startCond);
    ::pthread_cond_destroy(&_cond);
    ::pthread_mutex_destroy(&_mutex);
}

void SymbEventLoop::RunL()
{
    if (iStatus.Int() == KErrNone)
    {
        ProcessEvents();    
        Watch();
    }
}

void SymbEventLoop::DoCancel()
{
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrCancel);
}

void SymbEventLoop::Watch()
{
    if (_running)
    {
        ::pthread_mutex_lock(&_mutex);
        iStatus = KRequestPending;
        SetActive();        
        ::pthread_cond_signal(&_cond);
        ::pthread_mutex_unlock(&_mutex);            
    }
}

bool SymbEventLoop::CreateThread()
{
    int rc = ::pthread_create(&_thread, NULL, EventLoopThreadEntry, 
            reinterpret_cast<void*>(this));
    
    if (!rc)
    {
        ::pthread_mutex_lock(&_mutex);
        ::pthread_cond_wait(&_startCond, &_mutex);
        ::pthread_mutex_unlock(&_mutex);
        return true;
    }

    return false;    
}

bool SymbEventLoop::Start()
{
    CActiveScheduler::Add(this);
    
    ProcessQueuedEvents();
    
    return true;
}

bool SymbEventLoop::Stop()
{
    if (!_running)
        return false;
    
    _running = false;

    Wake();
    
    Deque();
    
    ::pthread_cond_signal(&_cond);
    
    int rc = ::pthread_join(_thread, NULL);

    return rc == 0;
}

void SymbEventLoop::CommitEvent(const Pt::Event& event)
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

void SymbEventLoop::QueueEvent(const Pt::Event& event)
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

void SymbEventLoop::Wake()
{
    //Pt::System::MutexLock lock( _wakeMutex );
    
    //_wakeMutexCond.signal();
    
    _selector.wake();
}

void SymbEventLoop::ProcessQueuedEvents()
{
    Pt::System::MutexLock lock(_processMutex);
    
    while (true)
    {
        _queueMutex.lock();

        if( _eventQueue.empty() )
        {
            _queueMutex.unlock();
            break;
        }

        Pt::Event* ev = _eventQueue.front();
        _eventQueue.remove(ev);

        _queueMutex.unlock();
        
        _appImpl.dispatchEvent(*ev);        
        delete ev;
    }    
}

void SymbEventLoop::ProcessEvents()
{
    Pt::System::MutexLock lock(_processMutex);

    while ( true == _running )
    {
        _queueMutex.lock();

        if( _eventQueue.empty() )
        {
            _queueMutex.unlock();
            break;
        }

        Pt::Event* ev = _eventQueue.front();
        _eventQueue.remove(ev);

        _queueMutex.unlock();

        _appImpl.dispatchEvent(*ev);
        
        delete ev;
    }
}

void* SymbEventLoop::EventLoopThreadEntry(void* threadID)
{
    SymbEventLoop* self = 
        reinterpret_cast<SymbEventLoop*>(threadID);

    self->EventLoopThread();

    ::pthread_exit(0);

    return 0;
}

void SymbEventLoop::EventLoopThread()
{   
    RThread thread;
    thread.Open(_mainThreadId);
    
    _running = true;
        
    ::pthread_cond_signal(&_startCond);

    while (_running)
    {
        //_wakeMutex.lock();

        ::pthread_mutex_lock(&_mutex);
        ::pthread_cond_wait(&_cond, &_mutex);
        
        _queueMutex.lock();

        if (_eventQueue.empty() && _running)
        {
            _queueMutex.unlock();

            //_wakeMutexCond.wait(_wakeMutex);

            _selector.wait(Pt::System::Selector::WaitInfinite);
        }
        else
        {
            _queueMutex.unlock();
        }
        

        TRequestStatus* status = &iStatus;
        if (*status == KRequestPending)
        {
            thread.RequestComplete(status, KErrNone);
        }            

        ::pthread_mutex_unlock(&_mutex);        
        //_wakeMutex.unlock();
    }

    ::pthread_mutex_lock(&_mutex);
    _running = false;
    ::pthread_mutex_unlock(&_mutex);

    thread.Close();
}

