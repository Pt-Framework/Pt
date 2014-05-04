/* Copyright (C) 2014 Marc Boris Dürner
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

#include "ApplicationImpl.h"
#include <Pt/System/IOError.h>
#include <Pt/System/Selectable.h>

namespace Pt {

namespace Qt {

ApplicationImpl::ApplicationImpl(int argc, char** argv)
: Pt::System::EventLoop()
, QApplication(argc, argv)
, _overlappedNotifier( _selector.overlappedEvent() )
, _wakeNotifier( _selector.wakeEvent() )
{
    connect(&_overlappedNotifier, SIGNAL(activated(HANDLE)), this, SLOT(onOverlapped(HANDLE)));
    connect(&_wakeNotifier, SIGNAL(activated(int)), this, SLOT(onWake(int)));
    connect(&_masterTimer, SIGNAL(timeout()), this, SLOT(processTimers()));

    _masterTimer.setSingleShot(true);
}


ApplicationImpl::~ApplicationImpl()
{
}


void ApplicationImpl::onAttachSelectable(System::Selectable& s)
{ 
    _selector.attach(s); 
}


void ApplicationImpl::onDetachSelectable(System::Selectable& s)
{ 
    _selector.detach(s); 
}


void ApplicationImpl::onOverlapped(HANDLE)
{
    _selector.runOverlapped();
}


void ApplicationImpl::onWake(int fd)
{
    bool isReady = _selector.isWoken();

    if( ! isReady )
        return;

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
        QApplication::quit();
}


void ApplicationImpl::onCancel(System::Selectable& s)
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


void ApplicationImpl::onReady(System::Selectable& s)
{
    Pt::System::MutexLock lock(_mutex);
    _avail.push_back(&s);

    // this is not neccessary if we can check _avail before the
    // Qaaplication starts to wait on the handles
    _selector.wake();
}


void ApplicationImpl::onRun()
{
    QApplication::exec();
}


void ApplicationImpl::onExit()
{
    _eventQueue.exit();
    wake();
}


void ApplicationImpl::onCommitEvent(const Pt::Event& ev)
{ 
    _eventQueue.pushEvent(ev);
    _selector.wake();
}


void ApplicationImpl::onQueueEvent(const Pt::Event& ev)
{ 
    _eventQueue.pushEvent(ev); 
}


void ApplicationImpl::onWake()
{
    _selector.wake();
}


void ApplicationImpl::onAttachTimer(System::Timer& timer)
{ 
    _timerQueue.addTimer(timer); 
    this->processTimers();
}


void ApplicationImpl::onDetachTimer(System::Timer& timer )
{ 
    _timerQueue.removeTimer(timer);
    this->processTimers();
}


void ApplicationImpl::processTimers()
{ 
    std::size_t nextTimer = _timerQueue.processTimers();

    if(nextTimer != System::EventLoop::WaitInfinite)
    {
        _masterTimer.start(nextTimer);
    }
}

} // namespace

} // namespace
