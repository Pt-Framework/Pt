/* Copyright (C) 2015-2025 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include "ApplicationImpl.h"

#include <Pt/Gfx/ImageSurface.h>
#include <Pt/System/Logger.h>
#include <Pt/System/Clock.h>
#include <Pt/DateTime.h>

PT_LOG_DEFINE("Pt.Hmi.Application")

namespace Pt {

namespace Hmi {

ApplicationImpl::ApplicationImpl()
: _lastActivityTime( Pt::System::Clock::getSystemTime() )
{
    std::cout << "ApplicationImpl()" << std::endl;
}


ApplicationImpl::~ApplicationImpl()
{
    std::cout << "~ApplicationImpl()" << std::endl;
} 


void ApplicationImpl::setCursor(const Cursor* cursor)
{

}


void ApplicationImpl::setFontDir(const Pt::System::Path& dir)
{
    Gfx::ImageSurface::setFontDir(dir);
}


void ApplicationImpl::setDefaultFont(const std::string& fontName)
{
    Gfx::ImageSurface::setDefaultFont(fontName);
}


Pt::Timespan ApplicationImpl::inactivityTime() const
{
    Pt::DateTime now = Pt::System::Clock::getSystemTime();
    Pt::Timespan inactivity = now - _lastActivityTime;
    return inactivity;
}	


void ApplicationImpl::sendKeyEvent(const KeyEvent& ev)
{
    _lastActivityTime = Pt::System::Clock::getSystemTime();
}


void ApplicationImpl::sendMouseEvent(const MouseEvent& ev)
{
}


void ApplicationImpl::nextEvent()
{
    //MainLoop::waitNext();
}


void ApplicationImpl::onAttachSelectable(System::Selectable&)
{
}


void ApplicationImpl::onDetachSelectable(System::Selectable&)
{
}


void ApplicationImpl::onCancel(System::Selectable& s)
{
}


void ApplicationImpl::onReady(System::Selectable& s)
{
}


void ApplicationImpl::onRun()
{
    std::cout << "ApplicationImpl::onRun" << std::endl;
}


void ApplicationImpl::onExit()
{
}


void ApplicationImpl::onCommitEvent(const Pt::Event& ev)
{
}


void ApplicationImpl::onQueueEvent(const Pt::Event& ev)
{
}


void ApplicationImpl::onWake()
{
}


void ApplicationImpl::onProcessEvents()
{
}


void ApplicationImpl::onAttachTimer(System::Timer& timer)
{
}


void ApplicationImpl::onDetachTimer(System::Timer& timer)
{
}

} // namespace

} // namespace
