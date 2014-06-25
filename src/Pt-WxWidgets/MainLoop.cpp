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
#include <Pt/WxWidgets/MainLoop.h>

namespace Pt {

namespace WxWidgets {

MainLoop::MainLoop(wxEventLoopBase& wxLoop)
: System::EventLoop()
, _impl(0) 
{
    _impl = new MainLoopImpl(wxLoop, this->eventReceived());
}


MainLoop::~MainLoop()
{
    delete _impl;
}


Pt::System::Selector& MainLoop::selector()
{
    return _impl->selector();
}


void MainLoop::onAttachSelectable(System::Selectable& s)
{ 
    _impl->attachSelectable(s);
}


void MainLoop::onDetachSelectable(System::Selectable& s)
{ 
    _impl->detachSelectable(s);
}


void MainLoop::onCancel(System::Selectable& s)
{
    _impl->cancel( s);
}


void MainLoop::onReady(System::Selectable& s)
{
    _impl->ready(s);
}


void MainLoop::onRun()
{
    _impl->run();
}


void MainLoop::onExit()
{
    _impl->exit();
}


void MainLoop::onCommitEvent(const Pt::Event& ev)
{ 
    _impl->commitEvent(ev);
}


void MainLoop::onQueueEvent(const Pt::Event& ev)
{ 
    _impl->queueEvent(ev);
}


void MainLoop::onWake()
{
    _impl->wake();
}


void MainLoop::onAttachTimer(System::Timer& timer)
{ 
    _impl->attachTimer(timer);
}


void MainLoop::onDetachTimer(System::Timer& timer )
{ 
    _impl->detachTimer(timer);
}

} // namespace

} // namespace
