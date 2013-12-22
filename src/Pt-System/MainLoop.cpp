/*
 * Copyright (C) 2007- 2010 Marc Boris Duerner
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
#include <Pt/System/MainLoop.h>

namespace Pt {

namespace System {

MainLoop::MainLoop()
: EventLoop()
, _impl(0)
{
    _impl = new MainLoopImpl( this->eventReceived() );
}


MainLoop::MainLoop(Allocator& a)
: EventLoop()
, _impl(0)
{
    _impl = new MainLoopImpl(this->eventReceived(), a);
}


MainLoop::~MainLoop()
{
    delete _impl;
}


Selector& MainLoop::selector()
{
    return _impl->selector();
}


void MainLoop::onAttachSelectable(Selectable& s)
{
    _impl->attach(s);
}


void MainLoop::onDetachSelectable(Selectable& s)
{
    _impl->detach(s);
}


void MainLoop::onCancel(Selectable& s)
{
    _impl->idle(s);
}


void MainLoop::onReady(Selectable& s)
{
    _impl->avail(s);
}


void MainLoop::onRun()
{
    _impl->run();
}


void MainLoop::onExit()
{
    _impl->exit();
}


void MainLoop::onCommitEvent(const Event& ev)
{
    _impl->commitEvent(ev);
}


void MainLoop::onQueueEvent(const Event& ev)
{
    _impl->queueEvent(ev);
}


void MainLoop::onWake()
{
    _impl->wake();
}


bool MainLoop::waitNext()
{
	return _impl->waitNext();
}


void MainLoop::onAttachTimer(Timer& timer)
{
    _impl->attach(timer);
}


void MainLoop::onDetachTimer( Timer& timer )
{
    _impl->detach(timer);
}

} // namespace System

} // namespace Pt
