/* 
 * Copyright (C) 2014 Marc Boris Dürner
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

#include "MainLoopImpl.h"
#include <Pt/System/IOError.h>
#include <Pt/System/Selectable.h>

namespace Pt {

namespace WxWidgets {

MainLoopImpl::MainLoopImpl(wxEventLoopBase& wxLoop)
: _selector(wxLoop)
{
    // TODO:
    // monitor _selector.wakeFd() -> onWakeNotify
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
    // wxEventLoop starts to wait on the handles
    _selector.wake();
}


void MainLoopImpl::run()
{
    //QApplication::exec();
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


//void MainLoopImpl::onWakeNotify(int fd)
//{
//    bool isReady = _selector.isWoken();
//
//    if( ! isReady )
//        return;
//
//    while( true )
//    {
//        Pt::System::MutexLock lock(_mutex);
//
//        if( _avail.empty() )
//            break;
//
//        System::Selectable* s = _avail.back();
//        _avail.pop_back();
//        lock.unlock();
//
//        s->run();
//    }
//
//    bool isActive = _eventQueue.processEvents( this->eventReceived() );
//    if( ! isActive )
//        QApplication::quit();
//}

void MainLoopImpl::Notify()
{
    processTimers();
}


void MainLoopImpl::processTimers()
{ 
    std::size_t nextTimer = _timerQueue.processTimers();

    if(nextTimer != System::EventLoop::WaitInfinite)
    {
        unsigned maxInt = std::numeric_limits<int>::max();
        
        int interval = nextTimer > maxInt ? maxInt 
                                          : static_cast<int>(nextTimer);
        
        wxTimer::StartOnce(interval);
    }
}

} // namespace

} // namespace
