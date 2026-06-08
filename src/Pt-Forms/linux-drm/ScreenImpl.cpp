/* Copyright (C) 2026 Marc Boris Duerner

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
#include "DrmDisplay.h"
#include "ApplicationImpl.h"
#include "../generic/GenericGraphicsBackend.h"

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Cursor.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/PaintEvent.h>
#include <Pt/Forms/ResizeEvent.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/System/Clock.h>
#include <Pt/System/Logger.h>
#include <Pt/Math.h>

#include <algorithm>
#include <cmath>
#include <cstring>

PT_LOG_DEFINE("Pt.Forms.Screen")

namespace Pt {

namespace Forms {

ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _genericBackend(0)
, _drmDisplay( app.drmDisplay() )
, _parent(0)
, _dpi(96.0)
, _cursorPos(0, 0)
, _drawCursor(false)
{
    GraphicsBackend& graphics = Application::instance().graphicsBackend();
    _genericBackend = dynamic_cast<GenericGraphicsBackend*>(&graphics);
    if( ! _genericBackend )
        throw std::invalid_argument("invalid graphics");

    Gfx::SizeF size( _drmDisplay.width(),
                     _drmDisplay.height() );

    _pixmap.reset(size);

    Gfx::PaintContext ctx(_pixmap);
    Gfx::Painter painter(ctx);

    Gfx::RectF rect( Gfx::PointF(0, 0), _pixmap.size() );
    painter.setBrush( Gfx::Color(0, 0, 0) );
    painter.fillRect(rect);

    setSurface( &_pixmap, Gfx::PointF(0, 0) );
    setContent(&_workspace);

    Gfx::RectI screenRect( Gfx::PointI(0, 0),
                           Gfx::SizeI(_drmDisplay.width(), 
                                      _drmDisplay.height()) );
    updateScreen(screenRect);
}


ScreenImpl::~ScreenImpl()
{
    Form::setSurface(0, Gfx::PointF(0, 0));
}


void ScreenImpl::setParent(Screen* screen)
{
    _parent = screen;

    if( _parent )
    {
        Gfx::SizeF size( _drmDisplay.width(),
                         _drmDisplay.height() );

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


void ScreenImpl::setCapture(Widget* /*capture*/)
{
}

//
// Widget
//

Widget* ScreenImpl::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) )
        return 0;

    Widget* hit = Form::onHitTest(p);
    if( hit )
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
    if( _parent )
        _parent->repaint(rect);
}

//
// Enable
//

void ScreenImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);
}


void ScreenImpl::onEnable(bool e)
{
    Base::onEnable(e);
}

//
// Input
//

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

//
// Scaling
//

void ScreenImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);
}


void ScreenImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    Gfx::SizeF size( _drmDisplay.width(),
                     _drmDisplay.height() );

    size /= scaleFactor();

    _pixmap.setScaleFactor( scaleFactor() );

    if( _parent )
        _parent->onResize(*this, size);
}


void ScreenImpl::onRescale(double scaling)
{
    Base::onRescale(scaling);
}

//
// Painting
//

void ScreenImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& updateRect = ev.rect();

    if( updateRect.isEmpty() )
        return;

    Base::onProcessPaintEvent(ev);

    Pt::Gfx::RectF updateRectP = scaling().toPhysical(updateRect);

    Gfx::RectI updateRectI( Gfx::PointI( lround(updateRectP.x()),
                                         lround(updateRectP.y()) ),
                            Gfx::SizeI( lround(updateRectP.width()),
                                        lround(updateRectP.height()) ) );
    updateScreen(updateRectI);
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
    // repaint the update area
    Gfx::Painter painter(context);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);
    painter.setBrush( Pt::Gfx::Color(0, 0, 0) );
    painter.fillRect(rect);
}

//
// Screen output
//

const Gfx::Image& ScreenImpl::image() const
{
#ifdef PT_FORMS_DRM_VULKAN
    // image() is not used with Vulkan renderer
    static Gfx::Image dummy;
    return dummy;
#else
    return _genericBackend->image(_pixmap);
#endif
}


void ScreenImpl::updateScreen(const Gfx::RectI& r)
{
#ifdef PT_FORMS_DRM_VULKAN
    // zero-copy path: submit Vulkan commands and page-flip with DMA-BUF fbId
    _pixmap.impl()->sync();

    uint32_t fbId = _pixmap.impl()->frontBuffer().drmFbId();
    _drmDisplay.swapBuffers(fbId);

    _pixmap.impl()->finish();
#else
    _drmDisplay.blit( image().data(), r );

    if( _drawCursor )
        drawCursor();

    _drmDisplay.swapBuffers();
#endif
}

//
// Cursor
//

void ScreenImpl::setCursor(const Pt::Gfx::PointF& pos)
{
    const Cursor& cursor = Application::instance().impl()->cursor();
    Gfx::SizeI cursorSize( cursor.width(), cursor.height() );

    Gfx::RectI dirtyArea( _cursorPos, cursorSize );

    _drawCursor = true;
    _cursorPos = Gfx::PointI( pos.x() - cursor.xHotspot(),
                              pos.y() - cursor.yHotspot() );

    PT_LOG_DEBUG("cursor position: " << _cursorPos.x() << "," << _cursorPos.y());

    dirtyArea.unify( Gfx::RectI(_cursorPos, cursorSize) );
    updateScreen(dirtyArea);
}


void ScreenImpl::drawCursor()
{
    const Cursor& cursor = Application::instance().impl()->cursor();
    if( cursor.empty() )
        return;

    const int dstW = static_cast<int>( _drmDisplay.width() );
    const int dstH = static_cast<int>( _drmDisplay.height() );
    const int srcW = static_cast<int>( cursor.width() );
    const int srcH = static_cast<int>( cursor.height() );

    // clip cursor rect to screen bounds
    int x0 = std::max(0, _cursorPos.x());
    int y0 = std::max(0, _cursorPos.y());
    int x1 = std::min(dstW, _cursorPos.x() + srcW);
    int y1 = std::min(dstH, _cursorPos.y() + srcH);

    if( x0 >= x1 || y0 >= y1 )
        return;

    Pt::uint8_t* dst = _drmDisplay.backBuffer();
    const Pt::uint8_t* andMask = &cursor.andRgb888()[0];
    const Pt::uint8_t* xorMask = &cursor.xorRgb888()[0];

    const int dstPitch = dstW * 4;
    const int srcPitch = srcW * 4;
    const int srcOffX = x0 - _cursorPos.x();
    const int srcOffY = y0 - _cursorPos.y();

    for(int y = y0; y < y1; ++y)
    {
        Pt::uint32_t* dstRow = reinterpret_cast<Pt::uint32_t*>(dst + y * dstPitch) + x0;
        const Pt::uint32_t* andRow = reinterpret_cast<const Pt::uint32_t*>(
            andMask + (y - y0 + srcOffY) * srcPitch) + srcOffX;
        const Pt::uint32_t* xorRow = reinterpret_cast<const Pt::uint32_t*>(
            xorMask + (y - y0 + srcOffY) * srcPitch) + srcOffX;

        for(int x = 0; x < (x1 - x0); ++x)
        {
            dstRow[x] = (dstRow[x] & andRow[x]) ^ xorRow[x];
        }
    }
}

} // namespace

} // namespace
