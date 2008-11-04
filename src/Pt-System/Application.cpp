/***************************************************************************
 *   Copyright (C) 2008 Marc Boris Duerner                                 *
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
#include "ApplicationImpl.h"
#include "Pt/System/Application.h"
#include "Pt/Event.h"
#include <string>
#include <iostream>
#include <stdexcept>

namespace {

Pt::System::Application*& getSystemAppPtr()
{
	static Pt::System::Application* _app = 0;
	return _app;
}

}

namespace Pt {

namespace System {

Application::Application(int argc, char** argv)
: Pt::Application(argc, argv)
, _loop(0)
, _owner(0)
{
    _impl = new ApplicationImpl;

    // base class already throws if constructed twice
    ::getSystemAppPtr() = this;

    _owner = new EventLoop();
    this->init(*_owner);
}


Application::Application(EventLoopBase* loop, int argc, char** argv)
: Pt::Application(argc, argv)
, _loop(loop)
, _owner(0)
{
    _impl = new ApplicationImpl;

    if(_loop)
        this->init(*_loop);

    // base class already throws if constructed twice
    ::getSystemAppPtr() = this;
}


Application::~Application()
{
    delete _owner;
    ::getSystemAppPtr() = 0;

    delete _impl;
}


Application& Application::instance()
{
    Application* app = ::getSystemAppPtr();
    if( ! app )
        throw std::logic_error("application not initialized");

    return *app;
}


bool Application::catchSystemSignal(int sig)
{
    return _impl->catchSystemSignal(sig);
}


bool Application::raiseSystemSignal(int sig)
{
    return _impl->raiseSystemSignal(sig);
}


void Application::init(EventLoopBase& loop)
{
    _loop = &loop;
    _impl->init(*_loop);
}

} // namespace System

} // namespace Pt
