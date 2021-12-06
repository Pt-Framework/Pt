/* 
 * Copyright (C) 2014 Marc Boris Duerner
 * Copyright (C) 2014 Laurentiu-Gheorghe Crisan
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

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Pt/WinVer.h>
#include <Windows.h>

#include <Pt/Mfc/MainLoop.h>
#include <Pt/System/Selectable.h>
#include <Pt/System/SystemError.h>
#include <win32/Selector.h>

namespace Pt {

namespace Mfc {

MainLoop::MainLoop()
:_selector( new Pt::System::Selector() )
,_timer(NULL)
{
  _timer = CreateWaitableTimer(0, FALSE, 0);
   if ( _timer == NULL )
       throw Pt::System::SystemError("CreateWaitableTimer failed");
}


MainLoop::~MainLoop()
{
    delete _selector;
    CloseHandle(_timer);
}


void MainLoop::onAttachSelectable(System::Selectable& s)
{ 
    _selector->attach(s); 
}


void MainLoop::onDetachSelectable(System::Selectable& s)
{ 
    _selector->detach(s); 
}


void MainLoop::onCancel(System::Selectable& s)
{
    Pt::System::MutexLock lock(_mutex);

    std::vector<System::Selectable*>::iterator it = _avail.begin();
    while(it != _avail.end())
    {
        if(*it == &s)
            it = _avail.erase(it);
        else
            ++it;
    }
}


void MainLoop::onReady(System::Selectable& s)
{
    Pt::System::MutexLock lock(_mutex);
    _avail.push_back(&s);

    // this is not neccessary if we can check _avail before the
    // we start to wait on the handles
    _selector->wake();
}


void MainLoop::onCommitEvent(const Pt::Event& ev)
{ 
    _eventQueue.pushEvent(ev);
    _selector->wake();
}


void MainLoop::onQueueEvent(const Pt::Event& ev)
{ 
    _eventQueue.pushEvent(ev); 
}


void MainLoop::onProcessEvents()
{
    _eventQueue.processEvents( this->eventReceived() );
}


void MainLoop::onWake()
{
    _selector->wake();
}


void MainLoop::onAttachTimer(System::Timer& timer)
{ 
    _timerQueue.addTimer(timer); 
    this->processTimers();
}


void MainLoop::onDetachTimer(System::Timer& timer )
{ 
    _timerQueue.removeTimer(timer);
    this->processTimers();
}


void MainLoop::onRun()
{
}


void MainLoop::onExit()
{
    _eventQueue.exit();
    wake();
}


bool MainLoop::pumpMessage()
{
    HANDLE handles[] = { _selector->overlappedEvent(), 
                         _selector->wakeEvent(),
                         _timer };

    DWORD n = sizeof(handles) / sizeof(HANDLE);

    DWORD result = ::MsgWaitForMultipleObjectsEx(n, handles, INFINITE, QS_ALLINPUT,
                                                 MWMO_INPUTAVAILABLE | MWMO_ALERTABLE );

    if(result == WAIT_FAILED)
        throw Pt::System::SystemError("MsgWaitForMultipleObjectsEx");

    switch (result)
    {
        case WAIT_OBJECT_0:
            _selector->runOverlapped();
            break;

        case WAIT_OBJECT_0 + 1:
            this->handleWake();
            break;

        case WAIT_OBJECT_0 + 2: 
            processTimers();
            break;

        case WAIT_OBJECT_0 + 3: 
            return true;
            break;

        case WAIT_IO_COMPLETION:
            break;
    }

    return false;
}


void MainLoop::processTimers()
{ 
    std::size_t nextTimer = _timerQueue.processTimers();

    LARGE_INTEGER liDueTime;

    if(nextTimer != System::EventLoop::WaitInfinite)
    {
        liDueTime.QuadPart = nextTimer * 10000;
        if (!SetWaitableTimer(_timer, &liDueTime, 0, NULL, NULL, 0) )
          throw Pt::System::SystemError("SetWaitableTimer failed");
    }
    else
      CancelWaitableTimer(_timer);
}


void MainLoop::handleWake()
{
    while( true )
    {
        Pt::System::MutexLock lock(_mutex);

        if( _avail.empty() )
            break;

        System::Selectable* s = _avail.back();
        _avail.pop_back();
        lock.unlock();

        s->run();
    }

    bool isActive = _eventQueue.processEvents( this->eventReceived() );
    if( ! isActive )
      ::PostQuitMessage(0);
}

} // namespace

} // namespace
