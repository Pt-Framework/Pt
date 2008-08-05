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
#include "Pt/System/Application.h"
#include "Pt/Event.h"
#include <string>
#include <iostream>
#include <stdexcept>

/*
namespace {

    int signalPipe[2] = {-1, -1};

    void initSignalPipe()
    {
        if (signalPipe[0] == -1)
        {
            if (pipe(signalPipe) == -1)
            {
                throw std::runtime_error("error creating signal pipe");
            }

            int flags = ::fcntl(signalPipe[0], F_GETFL);
            if(-1 == flags)
                throw std::runtime_error("Could not get pipe flags." + CXXTOOLS_SOURCEINFO);

            int ret = ::fcntl(signalPipe[0], F_SETFL, flags|O_NONBLOCK);
            if(-1 == ret)
                throw std::runtime_error("Could not set pipe to non-blocking." + CXXTOOLS_SOURCEINFO);

            flags = ::fcntl(signalPipe[1], F_GETFL);
            if(-1 == flags)
                throw std::runtime_error("Could not get pipe flags." + CXXTOOLS_SOURCEINFO);

            ret = ::fcntl(signalPipe[1], F_SETFL, flags|O_NONBLOCK);
            if(-1 == ret)
                throw std::runtime_error("Could not set pipe to non-blocking." + CXXTOOLS_SOURCEINFO);

        }
    }

}

extern "C" void pt_application_sighandler(int sigNo)
{
    if (signalPipe[1] != -1)
        write(signalPipe[1], &sigNo, sizeof(sigNo));
}
*/


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
	// base class already throws if constructed twice
	::getSystemAppPtr() = this;

    _loop = new EventLoop();
    _owner = _loop;
    _loop->setApp(this);

    //::initSignalPipe();

    getAppPtr() = this;
}


Application::Application(EventLoopBase* loop, int argc, char** argv)
: Pt::Application(argc, argv)
, _loop(loop)
, _owner(0)
{
	// base class already throws if constructed twice
	::getSystemAppPtr() = this;

    //::initSignalPipe();

    getAppPtr() = this;
}


Application::~Application()
{
    delete _owner;
    ::getSystemAppPtr() = 0;
}


Application& Application::instance()
{
	Application* app = ::getSystemAppPtr();
	if( ! app )
		throw std::logic_error("application not initialized");

	return *app;
}


void Application::init(EventLoopBase& loop)
{
    if(_loop)
        throw std::logic_error("eventloop already set");

    _loop = &loop;
    _loop->setApp(this);
}


Application*& Application::getAppPtr()
{
    static Application* _app = 0;
    return _app;
}

/*
void Application::catchSystemSignal(int sig)
{
    if (sig > 0 && sig < NSIG)
    {
        struct sigaction act;

        act.sa_handler = pt_application_sighandler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_RESTART;

        if (-1 == sigaction(sig, &act, NULL))
        {
            throw SystemError("sigaction failed", CXXTOOLS_SOURCEINFO);
        }
    }
}

int Application::getSignalFd() const
{
    return signalPipe[0];
}
*/

} // namespace System

} // namespace Pt
