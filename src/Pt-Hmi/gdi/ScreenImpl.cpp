/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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

#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include "MainWindowImpl.h"
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Windows.h>

namespace Pt {

namespace Hmi {

ScreenImpl::ScreenImpl(ApplicationImpl&)
{
    HDC screen = GetDC(GetDesktopWindow());

    int dpix = GetDeviceCaps(screen, LOGPIXELSX);

    //std::clog << "SCALING DPI: " << dpix << std::endl;
    //std::clog << "SCALING: " << dpix / 96.0 << std::endl;

    _scalingFactor = dpix / 96.0;

    ReleaseDC(GetDesktopWindow(), screen);
}


ScreenImpl::~ScreenImpl()
{
}


void ScreenImpl::init(WindowBase& w)
{
}


double ScreenImpl::scaleFactor(const Window& w) const
{
    if( ! w.impl() )
        return 1.0;

    return w.impl()->scaleFactor();
}


double ScreenImpl::scaleFactor() const
{
    return _scalingFactor;
}


void ScreenImpl::registerWindow(Window& w)
{
}


void ScreenImpl::unregisterWindow(Window& w)
{
}

        
Gfx::PointF ScreenImpl::toParent(const Window& w, const Gfx::PointF& pos) const
{
    Gfx::PointF physicalPos = w.toPhysical(pos);
    Gfx::PointF parentPos = w.impl()->toScreen(physicalPos);
    Gfx::PointF logicalPos = w.toLogical(parentPos);
    return logicalPos;
}


Gfx::PointF ScreenImpl::fromParent(const Window& w, const Gfx::PointF& pos) const
{
    Gfx::PointF physicalPos = w.toPhysical(pos);
    Gfx::PointF windowPos = w.impl()->fromScreen(physicalPos);
    Gfx::PointF logicalPos = w.toLogical(windowPos);
    return logicalPos;
}


void ScreenImpl::dispatchMouseEvent(const MouseEvent& ev)
{
    Application::instance().loop().commitEvent(ev);
}


void ScreenImpl::dispatchTouchEvent(const TouchEvent& ev)
{
    Application::instance().loop().commitEvent(ev);
}


void ScreenImpl::dispatchScrollEvent(const ScrollEvent& ev)
{
    Application::instance().loop().commitEvent(ev);
}


void ScreenImpl::paint(const Gfx::RectF& updateRect)
{
}


void ScreenImpl::onResize(Window& w, const Gfx::SizeF& s)
{
}


void ScreenImpl::onMove(Window& w, const Gfx::PointF& p)
{
}


void ScreenImpl::onFrameChanged(Window& w)
{
}


void ScreenImpl::onStateChanged(Window& w)
{
}


void ScreenImpl::onClosing(Window& w)
{
}


void ScreenImpl::onClose(Window& w)
{
}


void ScreenImpl::onShow(Window& w, bool visible)
{
    w.impl()->show(visible);
}


void ScreenImpl::onActivate(Window& w, bool active)
{
}


void ScreenImpl::onEnable(Window& w, bool enable)
{
}


Gfx::SizeF ScreenImpl::size() const
{
    HWND hDesktop = GetDesktopWindow();

    RECT r;   
    GetWindowRect(hDesktop, &r);

    return Gfx::SizeF(r.right, r.bottom);
}

} // namespace

} // namespace
