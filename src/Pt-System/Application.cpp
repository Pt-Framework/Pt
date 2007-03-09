/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
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
using namespace std;


namespace Pt {

namespace System {

Application::Application()
{
    connect(_loop.event, this->event);
    //connect(_loop.event, *this, &Application::dispatchEvent);
}


int Application::run()
{
    return _loop.run();
}


void Application::exit()
{
    _loop.exit();
}


EventLoop& Application::eventLoop()
{
    return _loop;
}


void Application::commitEvent(const Pt::Event& ev)
{
    _loop.commitEvent(ev);
}


void Application::queueEvent(const Pt::Event& ev)
{
    _loop.queueEvent(ev);
}


void Application::processEvents()
{
    _loop.processEvents();
}

} // namespace System

} // namespace Pt
