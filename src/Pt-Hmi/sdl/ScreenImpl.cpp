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

#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include "PixmapImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Gfx/ImageSurface.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/System/Clock.h>
#include <Pt/System/Logger.h>
#include <Pt/Math.h>

#include <algorithm>
#include <cmath>
#include <cstring>

PT_LOG_DEFINE("Pt.Hmi.Screen")

namespace Pt {

namespace Hmi {

const unsigned screenWidth = 800;
const unsigned screenHeight = 600;

ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _parent(0)
, _screen(0)
{
    _screen = SDL_CreateWindow("Screen", 0, 0,
                               screenWidth, screenHeight, 0);

    Gfx::SizeF size(screenWidth, screenHeight);
    _pixmap.resize(size);
                             
    Gfx::PaintSurface* surface = _pixmap.surface();
    if(surface)
    {
      Gfx::Painter painter(*surface);

      Gfx::RectF rect( Gfx::PointF(0, 0), _pixmap.size() );
      painter.setBrush( Gfx::Color(0, 0, 0) );
      painter.fillRect(rect);
    }

    Form::setSurface(surface, Gfx::PointF(0, 0) );

    setContent(&_shell);
}


ScreenImpl::~ScreenImpl()
{
    Form::setSurface(0, Gfx::PointF(0, 0) );
}


void ScreenImpl::setParent(Screen* screen)
{
    _parent = screen;

    if(_parent)
    {
        Gfx::SizeF size(screenWidth, screenHeight);

        size /= scaleFactor();

        _parent->onResize(*this, size);
        _parent->onShow(*this, true);
    }

    onSetParent(_parent);
}


void ScreenImpl::addWindow(Window& w)
{
    _shell.addWindow(w); 
}


void ScreenImpl::removeWindow(Window& w)
{ 
    _shell.removeWindow(w); 
}


const std::vector<Window*>& ScreenImpl::windows() const
{
    return _shell.windows();
}


WindowManager& ScreenImpl::windowManager()
{
    return _shell.windowManager();
}


///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual* ScreenImpl::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) )
        return 0;

    Visual* hit = Form::onHitTest(p);
    if(hit)
        return hit;

    return 0;
}


Gfx::PointF ScreenImpl::onToParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->toParent(pos);
}


Gfx::PointF ScreenImpl::onFromParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->fromParent(pos);
}


void ScreenImpl::onProcessEvent(const Event& ev)
{
    Base::onProcessEvent(ev);
}


void ScreenImpl::onRequestRepaint(const Gfx::RectF& rect)
{
    if(_parent)
        _parent->repaint(rect);
}


void ScreenImpl::setCapture(Visual* capture)
{
}

///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

//bool ScreenImpl::isEnabled() const
//{
//    return _enabledState;
//}


void ScreenImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    //bool isEnabled = ev.enabled();
    //if( ! _enabled )
    //  isEnabled = false;

    Base::onProcessEnableEvent(ev);
}


void ScreenImpl::onEnable(bool e)
{
    Base::onEnable(e);
}


void ScreenImpl::onProcessMouseEvent(const MouseEvent& ev)
{
    Base::onProcessMouseEvent(ev);
}


bool ScreenImpl::onMouseEvent(const MouseEvent& ev)
{ 
    return Base::onMouseEvent(ev);
}


void ScreenImpl::onProcessTouchEvent(const TouchEvent& ev)
{ 
    Base::onProcessTouchEvent(ev);
}


bool ScreenImpl::onTouchEvent(const TouchEvent& ev)
{ 
    return Base::onTouchEvent(ev);
}


void ScreenImpl::onProcessScrollEvent(const ScrollEvent& ev)
{
    if( ! isEnabled() )
        return;
  
    Base::onProcessScrollEvent(ev);
}


bool ScreenImpl::onScrollEvent(const ScrollEvent& ev)
{
    return Base::onScrollEvent(ev);
}


void ScreenImpl::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! isEnabled() )
        return;
    
    Base::onProcessKeyEvent(ev);
}


bool ScreenImpl::onKeyEvent(const KeyEvent& ev)
{
    return Base::onKeyEvent(ev);
}


void ScreenImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{   
    Base::onProcessRescaleEvent(ev);
}


void ScreenImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    Gfx::SizeF size(screenWidth, screenHeight);
    
    size /= scaleFactor();

    _pixmap.setScaleFactor( scaleFactor() );

    if(_parent)
        _parent->onResize(*this, size);
}


void ScreenImpl::onRescale(double scaling)
{
    Base::onRescale(scaling);
}


void ScreenImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& updateRectF = ev.rect();

    if( updateRectF.isNull() )
        return;

    Base::onProcessPaintEvent(ev);

    std::cout << "PAINT EVENT" << std::endl;

    //Pt::Gfx::RectF updateRectP = scaling().toPhysical(updateRectF);
    //
    //Rect updateRect( Gfx::Image::Point( lround(updateRectP.x()), 
    //                                    lround(updateRectP.y()) ),
    //                 Gfx::Image::Size( lround(updateRectP.width()),
    //                                   lround(updateRectP.height()) ) );

    const Gfx::Image& image = _pixmap.impl()->toImage();

    SDL_Surface* rgbSurface = SDL_CreateRGBSurfaceWithFormatFrom( (void*) image.data(), 
                                                                  image.width(), image.height(), 
                                                                  32, image.width() * 4,
                                                                  SDL_PIXELFORMAT_RGB888 );

    SDL_Surface* surface = SDL_GetWindowSurface(_screen);

    if( SDL_MUSTLOCK(surface) ) 
        SDL_LockSurface(surface);

    int r = SDL_BlitSurface(rgbSurface, NULL, surface, NULL);

    if( SDL_MUSTLOCK(surface) ) 
        SDL_UnlockSurface(surface);

    SDL_UpdateWindowSurface(_screen);

    SDL_FreeSurface(rgbSurface);
}


void ScreenImpl::onPaintEvent(const PaintEvent& ev)
{    
    Base::onPaintEvent(ev);

    const Gfx::RectF& rect = ev.rect();

    Gfx::PaintSurface* surface = _pixmap.surface();
    if(surface)
        onPaint(*surface, rect);
}


void ScreenImpl::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    Gfx::Painter painter(surface);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);
    painter.setBrush( Pt::Gfx::Color(0, 0, 0) );
    painter.fillRect(rect);
}

} // namespace

} // namespace
