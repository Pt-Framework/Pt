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

#include "ApplicationImpl.h"
#include <Pt/Qt/Application.h>
#include <Pt/System/EventLoop.h>

namespace Pt {

namespace Qt {

Application::Application(int& argc, char** argv)
: System::Application(0, 0, 0)
, _impl(0) 
{
    // pass a copy of argc/argv to QApplication

    _impl =  new ApplicationImpl(argc, argv);
    Pt::System::Application::init( _impl->loop() );
}


Application::~Application()
{
    delete _impl;
}


Application& Application::instance()
{
    return static_cast<Application&>( System::Application::instance() );
}


ApplicationImpl* Application::impl()
{
    return _impl;
}


QApplication& Application::qApplication()
{
    return _impl->qApplication();
}

} // namespace

} // namespace
