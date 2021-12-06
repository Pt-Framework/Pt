/*
 * Copyright (C) 2014 Marc Boris Duerner
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
#include <Pt/Qt/MainLoop.h>

namespace Pt {

namespace Qt {

MainLoop::MainLoop(QCoreApplication& app)
: System::EventLoop()
, _impl(0) 
{
    _impl = new MainLoopImpl(app, this->eventReceived());
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


void MainLoop::onProcessEvents()
{
    _impl->processEvents();
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
