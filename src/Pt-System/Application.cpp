/*
 * Copyright (C) 2008 Marc Boris Duerner
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
#include <Pt/System/Application.h>
#include <Pt/Event.h>
#include <stdexcept>

namespace {

Pt::System::Application*& getSystemAppPtr()
{
	static Pt::System::Application* _app = 0;
	return _app;
}

} // naamespace

namespace Pt {

namespace System {

Application::Application(int argc, char** argv)
: _argc(argc)
, _argv(argv)
, _loop(0)
, _owner(0)
{
    Application* app = ::getSystemAppPtr();
    if( app )
        throw std::logic_error("application not initialized");

    ::getSystemAppPtr() = this;

    _impl = new ApplicationImpl;

    _owner = new MainLoop();
    this->init(*_owner);
}


Application::Application(EventLoop* loop, int argc, char** argv)
: _argc(argc)
, _argv(argv)
, _loop(0)
, _owner(0)
{
    ::getSystemAppPtr() = this;

    _impl = new ApplicationImpl;

    if(loop)
        this->init(*loop);
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


bool Application::ignoreSystemSignal(int sig)
{
    return _impl->ignoreSystemSignal(sig);
}


bool Application::catchSystemSignal(int sig)
{
    return _impl->catchSystemSignal(sig);
}


bool Application::raiseSystemSignal(int sig)
{
    return _impl->raiseSystemSignal(sig);
}


void Application::chdir(const std::string& path)
{
    ApplicationImpl::chdir(path);
}


std::string Application::cwd()
{
    return ApplicationImpl::cwd();
}


std::string Application::rootdir()
{
    return ApplicationImpl::rootdir();
}


std::string Application::tmpdir()
{
    return ApplicationImpl::tmpdir();
}


void Application::setEnvVar(const std::string& name, const std::string& value)
{
    ApplicationImpl::setEnvVar(name, value);
}


void Application::unsetEnvVar(const std::string& name)
{
     ApplicationImpl::unsetEnvVar(name);
}


std::string Application::getEnvVar(const std::string& name)
{
     return ApplicationImpl::getEnvVar(name);
}


unsigned long Application::usedMemory()
{
     return ApplicationImpl::usedMemory();
}


void Application::init(EventLoop& loop)
{
    _loop = &loop;
    _impl->init(*_loop);
}

} // namespace System

} // namespace Pt
