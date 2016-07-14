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
  MA  02110-1301  USA
*/

#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include "MainWindowImpl.h"
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>

namespace Pt {

namespace Hmi {

ScreenImpl::ScreenImpl(ApplicationImpl&)
: _dpi(96.0)
{
      _size = screenResolution();

    _width  = _size.width() * unitSizeInch()*_dpi;
    _height = _size.height() * unitSizeInch()*_dpi;
    
    _factorX = _width / _size.width();
    _factorY = _height / _size.height();

    _offsetX = 0;
    _offsetY = 0;
}


ScreenImpl::~ScreenImpl()
{
}


void ScreenImpl::init(WindowBase& w)
{

}


Gfx::PointF ScreenImpl::toParent(const Window& w, const Gfx::PointF& pos) const
{
    return w.impl()->toScreen(pos);
}


Gfx::PointF ScreenImpl::fromParent(const Window& w, const Gfx::PointF& pos) const
{
    return w.impl()->fromScreen(pos);
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


void ScreenImpl::onActivate(Window& w)
{
    //const std::vector<Window*>& windows = w.windows();
    //
    //std::vector<Window*>::const_iterator it;
    //for(it = windows.begin(); it != windows.end(); ++it)
    //{
    //    if((*it)->isActive() && *it != &w)
    //    {
    //        ActivateEvent aev( (*it)->vid(), false );
    //    }
    //}

    //Window* parent = w.parent();

    //if(parent)
    //    onActivate(*parent);

    //ActivateEvent aev( w.vid(), true );
    //Application::instance().loop().commitEvent(aev);
}


void ScreenImpl::onEnable(Window& w, bool enable)
{
}


double ScreenImpl::width() const
{
  const HWND hDesktop = GetDesktopWindow();
  RECT desktop;   
  GetWindowRect(hDesktop, &desktop);
  return desktop.right;
}


double ScreenImpl::height() const
{
    const HWND hDesktop = GetDesktopWindow();
    RECT desktop;
    GetWindowRect(hDesktop, &desktop);
    return desktop.bottom;
}


//void ScreenImpl::setResolution(double dpi)
//{
//    _dpi = dpi;
//}


double ScreenImpl::resolutionDPI() const
{
    return _dpi;
}

Gfx::PointF ScreenImpl::toUnit(const Gfx::Point& value)
{
    const double x = value.x() * _factorX  + _offsetX;
    const double y = value.y() * _factorY  + _offsetY;

    return Gfx::PointF(std::ceil(x),std::ceil(y));
}


Gfx::SizeF ScreenImpl::toUnit(const Gfx::Size& value)
{
    const double width = value.width() * _factorX  + _offsetX;
    const double height = value.height() * _factorY  + _offsetY;

    return Gfx::SizeF(std::ceil(width), std::ceil(height));
}


Gfx::Point ScreenImpl::fromUnit(const Gfx::PointF& value)
{
    double factorX = _size.width() / _width;
    double factorY = _size.height() / _height;
    int x = (int) ( value.x() * factorX); 
    int y = (int) ( value.y() * factorY);

    return Gfx::Point(x,y);
}


Gfx::Size ScreenImpl::fromUnit(const Gfx::SizeF& value)
{
    double factorX = _size.width() / _width;
    double factorY = _size.height() / _height;
    int width = (int) ( value.width() * factorX); 
    int height = (int) ( value.height() * factorY);
    return Gfx::Size(width,height);
}


Gfx::Rect ScreenImpl::fromUnit(const Gfx::RectF& value)
{
    Gfx::Rect rect( Gfx::Point( static_cast<int>(value.x()), 
                                static_cast<int>(value.y()) ),
                    Gfx::Size( static_cast<int>(value.width()), 
                               static_cast<int>(value.height()) ) );
    return rect;
}


double ScreenImpl::unitSizeInch() const
{
    return 1.0/96.0;
}


double ScreenImpl::unitSizeMm() const
{
    return 25.4 * unitSizeInch();
}


Gfx::Size ScreenImpl::screenResolution()
{
  RECT desktop;    
  GetWindowRect(GetDesktopWindow(), &desktop);

  return Gfx::Size( desktop.right, desktop.bottom );
}

} // namespace

} // namespace
