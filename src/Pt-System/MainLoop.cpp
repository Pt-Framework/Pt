/*
 * Copyright (C) 2007- 2010 Marc Boris Duerner
 * Copyright (C) 2007 Laurentiu-Gheorghe Crisan
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
#include "Pt/System/MainLoop.h"

namespace Pt {

namespace System {

MainLoop::MainLoop()
: EventLoop()
, _impl(0)
{
    _impl = new MainLoopImpl();
}


MainLoop::MainLoop(Allocator& a)
: EventLoop()
, _impl(0)
{
    _impl = new MainLoopImpl(a);
}


MainLoop::~MainLoop()
{
    delete _impl;
}


void MainLoop::onAttach(Selectable& s)
{
    _impl->attach(s);
}


void MainLoop::onDetach(Selectable& s)
{
    _impl->detach(s);
}


void MainLoop::onEnable( Selectable& s )
{
    _impl->enable(s);
}


void MainLoop::onDisable( Selectable& s )
{
    _impl->disable(s);
}


void MainLoop::onReinit(Selectable& s)
{
}


void MainLoop::onChanged(Selectable& s)
{
    _impl->changed(s);
}


void MainLoop::onRun()
{
    _impl->run();
}


Signal<const Event&>& MainLoop::onEvent()
{
    return _impl->event();
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


void MainLoop::onProcessEvents()
{
    _impl->processEvents();
}


void MainLoop::onWake()
{
    _impl->wake();
}


void MainLoop::onAddTimer(Timer& timer)
{
    _impl->addTimer(timer);
}


void MainLoop::onRemoveTimer( Timer& timer )
{
    _impl->removeTimer(timer);
}

} // namespace System

} // namespace Pt
