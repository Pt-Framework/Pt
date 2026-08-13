/*
 * Copyright (C) 2014 Marc Boris Dürner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 * 02110-1301, USA.
 */

#include "MainLoopImpl.h"
#include <Pt/System/IOError.h>
#include <Pt/System/Selectable.h>

namespace Pt {

namespace Qt {

IONotifier::IONotifier(System::IOHandle& h)
: _h(&h)
, _readNotifier(h.fd, QSocketNotifier::Read)
, _writeNotifier(h.fd, QSocketNotifier::Write)
{ 
    _readNotifier.setEnabled(false);
    connect(&_readNotifier, SIGNAL(activated(int)), this, SLOT(onRead(int)));

    _writeNotifier.setEnabled(false);
    connect(&_writeNotifier, SIGNAL(activated(int)), this, SLOT(onWrite(int)));
}


MainLoopImpl::MainLoopImpl(QCoreApplication& app, Signal<const Pt::Event&>& eventSignal)
: _app(app)
, _event(eventSignal)
, _wakeNotifier( _selector.wakeFd(), QSocketNotifier::Read )
{
    connect(&_wakeNotifier, SIGNAL(activated(int)), this, SLOT(onWakeNotify(int)));
    connect(&_masterTimer, SIGNAL(timeout()), this, SLOT(processTimers()));

    _masterTimer.setSingleShot(true);
}


MainLoopImpl::~MainLoopImpl()
{
}


void MainLoopImpl::attachSelectable(System::Selectable& s)
{ 
    _selector.attach(s); 
}


void MainLoopImpl::detachSelectable(System::Selectable& s)
{ 
    _selector.detach(s); 
}


void MainLoopImpl::cancel(System::Selectable& s)
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


void MainLoopImpl::ready(System::Selectable& s)
{
    Pt::System::MutexLock lock(_mutex);
    _avail.push_back(&s);

    // this is not neccessary if we can check _avail before the
    // QApplication starts to wait on the handles
    _selector.wake();
}


void MainLoopImpl::run()
{
    _app.exec();
}


void MainLoopImpl::exit()
{
    _eventQueue.exit();
    wake();
}


void MainLoopImpl::commitEvent(const Pt::Event& ev)
{ 
    _eventQueue.pushEvent(ev);
    _selector.wake();
}


void MainLoopImpl::queueEvent(const Pt::Event& ev)
{ 
    _eventQueue.pushEvent(ev); 
}


void MainLoopImpl::processEvents()
{
    _eventQueue.processEvents(_event);
}


void MainLoopImpl::wake()
{
    _selector.wake();
}


void MainLoopImpl::attachTimer(System::Timer& timer)
{ 
    _timerQueue.addTimer(timer); 
    this->processTimers();
}


void MainLoopImpl::detachTimer(System::Timer& timer )
{ 
    _timerQueue.removeTimer(timer);
    this->processTimers();
}


void MainLoopImpl::onWakeNotify(int fd)
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

    bool isActive = _eventQueue.processEvents( _event );
    if( ! isActive )
        _app.quit();
}


void MainLoopImpl::processTimers()
{ 
    std::size_t nextTimer = _timerQueue.processTimers();

    if(nextTimer != System::EventLoop::WaitInfinite)
    {
        unsigned maxInt = std::numeric_limits<int>::max();
        
        int interval = nextTimer > maxInt ? maxInt 
                                          : static_cast<int>(nextTimer);

        _masterTimer.start(interval);
    }
}

} // namespace

} // namespace
