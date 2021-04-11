 /*
  Copyright (C) 2015 Marc Boris Duerner
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
#include "FrameBuffer.h"
#include "ApplicationImpl.h"
#include "MainWindowImpl.h"
#include "PixmapSurfaceImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Gfx/ImageSurface.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/System/Clock.h>
#include <Pt/System/Logger.h>

#include <algorithm>
#include <cmath>
#include <cstring>

PT_LOG_DEFINE("Pt.Hmi.Screen")

namespace Pt {

namespace Hmi {

ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _frameBuffer( app.frameBuffer() )
, _screen(0)
, _cursorPos(0, 0)
, _dpi(96.0)
, _drawCursor(false)
{
    _surface.pixmapImpl()->resize( _frameBuffer.size(), 
                                   _frameBuffer.strideSize() );

    Gfx::Image& background = _surface.pixmapImpl()->image();
    Gfx::ImageSurface backgroundSurface(background);
    Gfx::Painter painter(backgroundSurface);

    Gfx::RectF rect( Gfx::PointF(0, 0), _surface.size() );
    painter.setBrush( Gfx::Color(0, 0, 0) );
    painter.fillRect(rect);

    updateScreen( Gfx::Rect(Gfx::Point(0, 0), _frameBuffer.size()) );
}


ScreenImpl::~ScreenImpl()
{
}


void ScreenImpl::init(Screen& screen)
{
    _screen = &screen;
    _windowManager.init(screen);
}


void ScreenImpl::registerWindow(Window& w)
{
    _windowManager.add(w);
}


void ScreenImpl::unregisterWindow(Window& w)
{
    _windowManager.remove(w);
}


const Gfx::Image& ScreenImpl::image() const
{
    return _surface.pixmapImpl()->image();
}


Gfx::Image& ScreenImpl::image()
{
    return _surface.pixmapImpl()->image();
}


void ScreenImpl::paint(const Gfx::RectF& updateRect)
{
    //
    // erase previous cursor area in back buffer
    //
    if( ! _cursorBackground.empty() )
    {
        bitBlit( _cursorBackground.data(),
                 _cursorBackground.width(),
                 _cursorBackground.height(),
                 _cursorPos,
                 image().data(), CopyOp );
    }

    //
    // repaint the update area
    //
    Gfx::Painter painter(_surface);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);
    painter.setBrush( Pt::Gfx::Color(0, 0, 0) );
    painter.fillRect(updateRect);

    _windowManager.paint(_surface, updateRect);

    //std::clog << "screen update2: " << clock.stop().toUSecs() << " usecs." << std::endl;
    //std::clog << "update area2: " << updateRect.topLeft().x() << ',' << updateRect.topLeft().y()
    //          << ' ' << updateRect.width() << 'x' << updateRect.height() << std::endl;

    //
    // update the screen including the cursor
    //
    Pt::Gfx::RectF urect  = Pt::Hmi::Application::instance().screen().toPhysical(updateRect);
    updateScreen( Gfx::round(urect) );
}


void ScreenImpl::drawCursor(const Pt::Hmi::MouseEvent& mev)
{
    _drawCursor = true;
 
    //
    // erase previous cursor area on screen
    //
    if( ! _cursorBackground.empty() )
    {
        PT_LOG_DEBUG("erasing cursor image");

        bitBlit( _cursorBackground.data(), 
                 _cursorBackground.width(), _cursorBackground.height(),
                 _cursorPos, (Pt::uint8_t*)image().data(), CopyOp );

       // TODO: is this enough to clear the cursor area in the back buffer?
       _frameBuffer.output( image().data(), 
                            Gfx::Rect(_cursorPos, _cursorBackground.size()) );
    }

    //
    // calculate new cursor area
    //
    const Cursor& cursor = Application::instance().impl()->cursor();

    _cursorPos = Gfx::Point( mev.x() - cursor.xHotspot(),
                             mev.y() - cursor.yHotspot() );

    PT_LOG_DEBUG("cursor hotspot position: " << _cursorPos.x() << "," << _cursorPos.y());

    Gfx::Rect cursorArea = Gfx::Rect(_cursorPos,
                                     Gfx::Size(cursor.width(), cursor.height()));

    //
    // update the screen including the new cursor image
    //
    PT_LOG_DEBUG("update cursor area: " << cursor.width() << "x" << cursor.height());
    updateScreen(cursorArea);
}


void ScreenImpl::dispatchMouseEvent(const MouseEvent& ev)
{
    _windowManager.mouseEvent(ev);
}


void ScreenImpl::dispatchTouchEvent(const TouchEvent& ev)
{
    _windowManager.touchEvent(ev);
}


void ScreenImpl::dispatchScrollEvent(const ScrollEvent& ev)
{
    _windowManager.scrollEvent(ev);
}


void ScreenImpl::dispatchKeyEvent(const Pt::Hmi::KeyEvent& ev)
{
    _windowManager.keyEvent(ev);
}


Gfx::PointF ScreenImpl::toParent(const Window& w, const Gfx::PointF& pos) const
{
    //return w.impl()->toScreen(pos);
    return _windowManager.toParent(w, pos);
}


Gfx::PointF ScreenImpl::fromParent(const Window& w, const Gfx::PointF& pos) const
{
    //return w.impl()->fromScreen(pos);
    return _windowManager.fromParent(w, pos);
}


void ScreenImpl::onResize(Window& w, const Gfx::SizeF& s)
{
    ResizeEvent rev( w.vid(), s );
    Application::instance().loop().commitEvent(rev);

    _windowManager.onResize(w, s);
}


void ScreenImpl::onMove(Window& w, const Gfx::PointF& to)
{
    Gfx::RectF updateRect = _windowManager.frameRect(w);

    _windowManager.onMove(w, to);

    Gfx::RectF movedRect = _windowManager.frameRect(w);
    updateRect.unify(movedRect);

    MoveEvent mev( w.vid(), to );
    Application::instance().loop().commitEvent(mev);

    if(_screen)
      _screen->update(updateRect);
}


void ScreenImpl::onFrameChanged(Window& w)
{
    Gfx::RectF updateRect = _windowManager.frameRect(w);
    
    _windowManager.onFrameChanged(w);

    Gfx::RectF changedRect = _windowManager.frameRect(w);
    updateRect.unify(changedRect);

    if(_screen)
      _screen->update(updateRect);
}


void ScreenImpl::onStateChanged(Window& w)
{
    _windowManager.onStateChanged(w);
}


void ScreenImpl::onClosing(Window& w)
{
    _windowManager.onClosing(w);
}


void ScreenImpl::onClose(Window& w)
{
    Gfx::RectF updateRect = _windowManager.frameRect(w);

    // the window has been closed, clean up
    _windowManager.onClose(w);

    if(_screen)
      _screen->update(updateRect);
}


void ScreenImpl::onShow(Window& w, bool visible)
{
    _windowManager.onShow(w, visible);
}


void ScreenImpl::onActivate(Window& w, bool active)
{
    _windowManager.onActivate(&w, active);
}


void ScreenImpl::onEnable(Window& w, bool enable)
{
    _windowManager.onEnable(w, enable);
}


void ScreenImpl::updateScreen(const Gfx::Rect& r)
{
    if(_drawCursor)
        drawCursor( image().data() );
    
    _frameBuffer.output( image().data(), r );
}


void ScreenImpl::drawCursor(Pt::uint8_t* buffer)
{
    const Cursor& cursor = Application::instance().impl()->cursor();
    if( cursor.empty() )
    {
        PT_LOG_DEBUG("no cursor image");
        return;
    }
    
    if( _cursorBackground.width() != cursor.width() ||
        _cursorBackground.height() != cursor.height() )
    {
        Gfx::Size size( cursor.width(), cursor.height() );
        _cursorBackground.reset(_frameBuffer.format(), size);
    }

    // keep the of background of the cursor area
    PT_LOG_DEBUG("saving cursor background");
    grabImage( buffer, _cursorPos, _cursorBackground );

    // draw cursor to the buffer
    PT_LOG_DEBUG("drawing cursor image");

    bitBlit(&cursor.andRgb888()[0], cursor.width(), cursor.height(), 
            _cursorPos, buffer, AndOp);
    
    bitBlit(&cursor.xorRgb888()[0], cursor.width(), cursor.height(), 
            _cursorPos, buffer, XorOp);
}


void ScreenImpl::grabImage(const Pt::uint8_t* buffer, 
                           const Gfx::Point& pos, 
                           Gfx::Image& image)
{
    const size_t pixelSizeInByte = _frameBuffer.pixelSize();
    const Gfx::Size& imageSize = image.size();
    const size_t yMax = std::min<size_t>(pos.y() + imageSize.height(), _frameBuffer.height() );

    size_t widthInPixel = (pos.x() + imageSize.width()) < _frameBuffer.width() ? imageSize.width()
                                                                               : _frameBuffer.width() - pos.x();
    const size_t widthInByte = widthInPixel * pixelSizeInByte;

    for( Pt::ssize_t y = pos.y(); y < yMax; ++y )
    {
        size_t lineOffset = y * _frameBuffer.lineSize() +
                            pos.x() * pixelSizeInByte;

        Pt::uint8_t* pdata = image.data() + (y - pos.y()) * image.view().stride();
        std::memcpy( pdata, &buffer[lineOffset], widthInByte );
    }
}


void ScreenImpl::bitBlit( const Pt::uint8_t* plane, size_t w, size_t h,
                          const Gfx::Point& pos, Pt::uint8_t* buffer, BlitOp op )
{
    static const size_t planePixelSize = 4;
    const size_t bufferPixelSize = _frameBuffer.pixelSize();
    const size_t bufferWidth  = std::min<size_t>( pos.x() + w, _frameBuffer.width() );
    const size_t bufferHeight = std::min<size_t>( pos.y() + h, _frameBuffer.height() );
    size_t yCursor = 0;
    size_t xCursor = 0;

    for( size_t yBuffer = pos.y(); yBuffer < bufferHeight; ++yBuffer, ++yCursor )
    {
        const size_t lineOffsetBuffer  = yBuffer * _frameBuffer.lineSize();
        const size_t lineOffsetCursor  = yCursor * (w * planePixelSize);

        xCursor = 0;

        for( size_t xBuffer = pos.x(); xBuffer < bufferWidth; ++xBuffer, ++xCursor  )
        {
            Pt::uint8_t* pointerBuffer = &((Pt::uint8_t*)buffer)[lineOffsetBuffer + (xBuffer * bufferPixelSize)];
            const Pt::uint8_t* pointerCursor = &plane[lineOffsetCursor + (xCursor * planePixelSize)];

            switch( _frameBuffer.depth() )
            {
                case 32:
                {
                    Pt::uint32_t* pixelBuffer = (Pt::uint32_t*) pointerBuffer;
                    const Pt::uint32_t* pixelCursor = (const Pt::uint32_t*) pointerCursor;

                    switch( op )
                    {
                        case AndOp:
                            *pixelBuffer &= *pixelCursor;
                        break;

                        case XorOp:
                            *pixelBuffer ^= *pixelCursor;
                        break;

                        case CopyOp://ToDo::optimize this with memcpy
                            *pixelBuffer = *pixelCursor;
                        break;
                    }
                    break;
                }

                case 16:
                    //TODO:
                    break;
            }
        }
    }
}

} // namespace

} // namespace
