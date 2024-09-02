/* Copyright (C) 2015 Marc Boris Duerner 
  
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
  
  You should have received a copy of the GNU Lesser General Public License 
  along with this library; if not, write to the Free Software Foundation, 
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/
 
#include <Pt/Hmi/WindowFrame.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/WindowStateEvent.h>

namespace Pt {

namespace Hmi {

WindowFrame::WindowFrame(WindowManager& wm, Window& window)
: _wm(wm)
, _window(window)
{
    eventReceived() += Pt::slot(*this, &WindowFrame::onProcessActivateEvent);
    eventReceived() += Pt::slot(*this, &WindowFrame::onProcessCloseEvent);
    eventReceived() += Pt::slot(*this, &WindowFrame::onProcessWindowStateEvent);
}


WindowFrame::~WindowFrame()
{
}


Window& WindowFrame::window()
{
    return _window;
}


const Window& WindowFrame::window() const
{
    return _window;
}


PixmapSurface& WindowFrame::surface()
{
    return _surface;
}


const PixmapSurface& WindowFrame::surface() const
{
    return _surface;
}


void WindowFrame::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);
}


void WindowFrame::onRescaleEvent(const RescaleEvent& ev)
{
    // TODO: the reported scale factor divided by the application
    //       scale factor is the window specific one

    _surface.setScaleFactor( ev.scaleFactor() );

    Base::onRescaleEvent(ev);
}


void WindowFrame::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);
}


void WindowFrame::onResizeEvent(const ResizeEvent& ev)
{
    Visual::onResizeEvent(ev);

    _surface.resize( ev.size() );
}


void WindowFrame::onProcessActivateEvent(const ActivateEvent& ev)
{
    onActivateEvent(ev);
}


void WindowFrame::onActivateEvent(const ActivateEvent& ev)
{
}


void WindowFrame::onProcessWindowStateEvent(const WindowStateEvent& ev)
{
    onWindowStateEvent(ev);
}


void WindowFrame::onWindowStateEvent(const WindowStateEvent& ev)
{
}


void WindowFrame::onProcessCloseEvent(const CloseEvent& ev)
{
    onCloseEvent(ev);
}


void WindowFrame::onCloseEvent(const CloseEvent& ev)
{
}

} // namespace

} // namespace
