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

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Cursor.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/PaintEvent.h>
#include <Pt/Forms/ResizeEvent.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/System/Logger.h>

#include <emscripten.h>
#include <emscripten/html5.h>

PT_LOG_DEFINE("Pt.Forms.Screen")

namespace {

EM_BOOL onCanvasResized(int eventType, const void *reserved, void *obj)
{
	  double width, height;
	  emscripten_get_element_css_size("canvas", &width, &height);
    //std::clog << "emscripten resize: " << width << " " << height << std::endl;

    //std::clog << "pixel ratio: " << emscripten_get_device_pixel_ratio() << std::endl;
    emscripten_set_canvas_size( int(width), int(height) );

    return true;
}

} // namespace

namespace Pt {

namespace Forms {

ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _parent(0)
, _screen(0)
{
    EmscriptenFullscreenStrategy strategy;
		strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT;
    strategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
		strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
		strategy.canvasResizedCallback = onCanvasResized;
		strategy.canvasResizedCallbackUserData = this; 
		
    //strategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF;

    //std::clog << "pixel ratio: " << emscripten_get_device_pixel_ratio() << std::endl;
    
    //emscripten_set_resize_callback(0, 0, false, emscWindowSizeChanged)

    emscripten_enter_soft_fullscreen("canvas", &strategy);

	  double width, height;
	  emscripten_get_element_css_size("canvas", &width, &height);

    Gfx::SizeF size(width, height);
    _pixmap.reset(size);
                             
    const Gfx::Image& image = _pixmap.impl()->bitmap().image();

    _imageSurface = SDL_CreateRGBSurfaceWithFormatFrom( (void*) image.data(), 
                                                         image.width(), image.height(), 
                                                         32, image.width() * 4,
                                                         SDL_PIXELFORMAT_RGB888 );

    _screen = SDL_CreateWindow("Screen", 0, 0, width, height, 
                               SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);

    setSurface( &_pixmap, Gfx::PointF(0, 0) );
    setContent(&_workspace);
}


ScreenImpl::~ScreenImpl()
{
    Form::setSurface(0, Gfx::PointF(0, 0) );

    SDL_FreeSurface(_imageSurface);
    SDL_DestroyWindow(_screen);
}


void ScreenImpl::setParent(Screen* screen)
{
    _parent = screen;

    if(_parent)
    {
        Gfx::SizeF size = _pixmap.size();
        size /= scaleFactor();

        _parent->onResize(*this, size);
        _parent->onShow(*this, true);
    }

    onSetParent(_parent);
}


void ScreenImpl::addWindow(Window& w)
{
    _workspace.addWindow(w); 
}


void ScreenImpl::removeWindow(Window& w)
{ 
    _workspace.removeWindow(w); 
}


const std::vector<Window*>& ScreenImpl::windows() const
{
    return _workspace.windows();
}


WindowManager& ScreenImpl::windowManager()
{
    return _workspace.windowManager();
}


void ScreenImpl::setCapture(Widget* capture)
{
}

///////////////////////////////////////////////////////////////////////
// Widget
///////////////////////////////////////////////////////////////////////

Widget* ScreenImpl::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) )
        return 0;

    Widget* hit = Form::onHitTest(p);
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


void ScreenImpl::onRequestResize(const Gfx::SizeF& s)
{
    if(_parent)
        _parent->onResize(*this, s);
}


///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

void ScreenImpl::onProcessEnableEvent(const EnableEvent& ev)
{
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

    Gfx::SizeF size = _pixmap.size();
    size /= scaleFactor();

    _pixmap.setScaleFactor( scaleFactor() );

    if(_parent)
        _parent->onResize(*this, size);
}


void ScreenImpl::onRescale(double scaling)
{
    Base::onRescale(scaling);
}


void ScreenImpl::onResizeEvent(const ResizeEvent& ev)
{
    if( size() == ev.size() )
    {
        return;
    }

    Base::onResizeEvent(ev);

    Gfx::SizeF size = scaling().toPhysical( ev.size() );
    _pixmap.reset(size);
    
    const Gfx::Image& image = _pixmap.impl()->bitmap().image();

    SDL_FreeSurface(_imageSurface);
    _imageSurface = 0; 

    _imageSurface = SDL_CreateRGBSurfaceWithFormatFrom( (void*) image.data(), 
                                                         image.width(), image.height(), 
                                                         32, image.width() * 4,
                                                         SDL_PIXELFORMAT_RGB888 );
}


void ScreenImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& updateRectF = ev.rect();

    if( updateRectF.isNull() )
        return;

    Base::onProcessPaintEvent(ev);

    //Pt::Gfx::RectF updateRectP = scaling().toPhysical(updateRectF);
    //
    //Rect updateRect( Gfx::Image::Point( lround(updateRectP.x()), 
    //                                    lround(updateRectP.y()) ),
    //                 Gfx::Image::Size( lround(updateRectP.width()),
    //                                   lround(updateRectP.height()) ) );

    SDL_Surface* surface = SDL_GetWindowSurface(_screen);

    if( SDL_MUSTLOCK(surface) ) 
        SDL_LockSurface(surface);

    int r = SDL_BlitSurface(_imageSurface, NULL, surface, NULL);
    if(r < 0)
        std::clog << "SDL_BlitSurface failed: " << r << std::endl;

    if( SDL_MUSTLOCK(surface) ) 
        SDL_UnlockSurface(surface);

    SDL_UpdateWindowSurface(_screen);
}


void ScreenImpl::onPaintEvent(const PaintEvent& ev)
{    
    Base::onPaintEvent(ev);

    const Gfx::RectF& rect = ev.rect();
    PaintContext ctx(_pixmap);
    onPaint(ctx, rect);
}


void ScreenImpl::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    Gfx::Painter painter(context);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);
    painter.setBrush( Gfx::Color(40, 50, 80) );
    painter.fillRect(rect);
}

} // namespace

} // namespace
