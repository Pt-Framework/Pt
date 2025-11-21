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
 
#include <Pt/Forms/WindowFrame.h>
#include <Pt/Forms/WindowManager.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/WindowStateEvent.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

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


Pixmap& WindowFrame::pixmap()
{
    return _pixmap;
}


const Pixmap& WindowFrame::pixmap() const
{
    return _pixmap;
}


void WindowFrame::getBitmap(Gfx::Bitmap& bitmap)
{
    Gfx::PointF contentPos = onFromWindow( _window, Gfx::PointF(0, 0) );
    Gfx::RectF contentRect( contentPos, _window.size() );

    contentRect = scaling().toPhysical(contentRect);

    _pixmap.getBitmap(bitmap, contentRect);
}


void WindowFrame::onConnect(Screen& screen)
{
    Base::onConnect(screen);
    
    _window.onConnect(screen);
}


void WindowFrame::onDisconnect()
{
    Base::onDisconnect();
    
    _window.onDisconnect();
}


void WindowFrame::onProcessPaintEvent(const PaintEvent& ev)
{
    Base::onProcessPaintEvent(ev);

    _pixmap.finish();
    _wm.surfaceChanged().send(_window);
}


void WindowFrame::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);
}


void WindowFrame::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);
}


void WindowFrame::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    _pixmap.setScaleFactor( ev.scaleFactor() );

    Gfx::SizeF pixmapSize = scaling().toPhysical( size() );
    _pixmap.reset(pixmapSize);
}


void WindowFrame::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);
}


void WindowFrame::onResizeEvent(const ResizeEvent& ev)
{
    Widget::onResizeEvent(ev);

    Gfx::SizeF pixmapSize = scaling().toPhysical( ev.size() );
    _pixmap.reset(pixmapSize);
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
