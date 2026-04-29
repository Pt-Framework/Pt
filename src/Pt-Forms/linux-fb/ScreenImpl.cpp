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
#include "PixmapImpl.h"

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Cursor.h>
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
: _frameBuffer( app.frameBuffer() )
, _parent(0)
, _dpi(96.0)
, _cursorPos(0, 0)
, _drawCursor(false)
{
    Gfx::SizeF size( _frameBuffer.width(), 
                     _frameBuffer.height() );
    _pixmap.impl()->reset( size, _frameBuffer.strideSize() );
                             
    Gfx::Painter painter(_pixmap);

    Gfx::RectF rect( Gfx::PointF(0, 0), _pixmap.size() );
    painter.setBrush( Gfx::Color(0, 0, 0) );
    painter.fillRect(rect);

    setSurface( &_pixmap, Gfx::PointF(0, 0) );
    setContent(&_workspace);

    updateScreen( Gfx::RectI(Gfx::PointI(0, 0), _frameBuffer.size()) );
}


ScreenImpl::~ScreenImpl()
{
    Form::setSurface(0, Gfx::PointF(0, 0) );
}


void ScreenImpl::setParent(Screen* screen)
{
    //
    // TODO: Should we call onConnect after the _parent ist set and before
    //       onshow and onResize are performed?
    //       1. establish pointer to parent
    //       2. connect to screen
    //       3. call onShow/onResize on screen
    //
    
    _parent = screen;

    if(_parent)
    {
        Gfx::SizeF size( _frameBuffer.width(), 
                         _frameBuffer.height() );

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


//double ScreenImpl::scaleFactor() const
//{
//    return _surface.scaleFactor();
//}

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

    Gfx::SizeF size( _frameBuffer.width(), 
                     _frameBuffer.height() );
    
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

    //std::clog << "screen update2: " << clock.stop().toUSecs() << " usecs." << std::endl;
    //std::clog << "update area2: " << updateRect.topLeft().x() << ',' << updateRect.topLeft().y()
    //          << ' ' << updateRect.width() << 'x' << updateRect.height() << std::endl;

    //
    // update the screen including the cursor
    //
    Pt::Gfx::RectF updateRectP = scaling().toPhysical(updateRectF);
    
    Gfx::RectI updateRect( Gfx::PointI( lround(updateRectP.x()), 
                                        lround(updateRectP.y()) ),
                           Gfx::SizeI( lround(updateRectP.width()),
                                       lround(updateRectP.height()) ) );
    updateScreen(updateRect);
}


void ScreenImpl::onPaintEvent(const PaintEvent& ev)
{    
    Base::onPaintEvent(ev);

    const Gfx::RectF& rect = ev.rect();
    onPaint(_pixmap, rect);
}


void ScreenImpl::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
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
                 const_cast<uint8_t*>(image().data()), CopyOp );
    }

    //
    // repaint the update area
    // TODO: unless full screen window covers the whole screen
    //
    Gfx::Painter painter(surface);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);
    painter.setBrush( Pt::Gfx::Color(0, 0, 0) );
    painter.fillRect(rect);
}


const Gfx::Image& ScreenImpl::image() const
{
    return _pixmap.impl()->bitmap().image();
}


void ScreenImpl::updateScreen(const Gfx::RectI& r)
{
    if(_drawCursor)
        drawCursor( const_cast<uint8_t*>( image().data() ) );
    
    _frameBuffer.output( image().data(), r );
}


void ScreenImpl::drawCursor(const Pt::Gfx::PointF& pos)
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
                            Gfx::RectI(_cursorPos, Gfx::SizeI(_cursorBackground.width(),
                                                              _cursorBackground.height() ) ) );
    }

    //
    // calculate new cursor area
    //
    const Cursor& cursor = Application::instance().impl()->cursor();

    _cursorPos = Gfx::PointI( pos.x() - cursor.xHotspot(),
                         pos.y() - cursor.yHotspot() );

    PT_LOG_DEBUG("cursor hotspot position: " << _cursorPos.x() << "," << _cursorPos.y());

    Gfx::RectI cursorArea = Gfx::RectI( _cursorPos, Gfx::SizeI(cursor.width(), 
                                                               cursor.height()) );

    //
    // update the screen including the new cursor image
    //
    PT_LOG_DEBUG("update cursor area: " << cursor.width() << "x" << cursor.height());
    updateScreen(cursorArea);
}


void ScreenImpl::drawCursor(Pt::uint8_t* buffer)
{
    const Cursor& cursor = Application::instance().impl()->cursor();
    if( cursor.empty() )
    {
        PT_LOG_DEBUG("no cursor image");
        return;
    }
    
    if( _cursorBackground.width() != static_cast<int>( cursor.width() ) ||
        _cursorBackground.height() != static_cast<int>( cursor.height() ) )
    {
        _cursorBackground.reset(cursor.width(), cursor.height(), _frameBuffer.format() );
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
                           const Gfx::PointI& pos, 
                           Gfx::Image& image)
{
    const size_t pixelSizeInByte = _frameBuffer.pixelSize();
    const Pt::ssize_t yMax = std::min<Pt::ssize_t>(pos.y() + image.height(), _frameBuffer.height() );

    int fbWidth = static_cast<int>( _frameBuffer.width() );

    size_t widthInPixel = ( pos.x() + image.width() ) < fbWidth ? image.width()
                                                                : fbWidth - pos.x();
    const size_t widthInByte = widthInPixel * pixelSizeInByte;

    for(Pt::ssize_t y = pos.y(); y < yMax; ++y)
    {
        if(y < 0)
            continue;

        size_t lineOffset = y * _frameBuffer.lineSize() +
                            pos.x() * pixelSizeInByte;

        Pt::uint8_t* pdata = image.data() + (y - pos.y()) * image.stride();
        std::memcpy( pdata, &buffer[lineOffset], widthInByte );
    }
}


void ScreenImpl::bitBlit( const Pt::uint8_t* plane, size_t w, size_t h,
                          const Gfx::PointI& pos, Pt::uint8_t* buffer, BlitOp op )
{
    static const size_t planePixelSize = 4;
    const size_t bufferPixelSize = _frameBuffer.pixelSize();
    const size_t bufferWidth  = std::min<size_t>( pos.x() + w, _frameBuffer.width() );
    const size_t bufferHeight = std::min<size_t>( pos.y() + h, _frameBuffer.height() );
    size_t yCursor = 0;
    size_t xCursor = 0;

    for( ssize_t yBuffer = pos.y(); yBuffer < bufferHeight; ++yBuffer, ++yCursor )
    {
        if(yBuffer < 0)
            continue;

        const size_t lineOffsetBuffer  = yBuffer * _frameBuffer.lineSize();
        const size_t lineOffsetCursor  = yCursor * (w * planePixelSize);

        xCursor = 0;

        for( ssize_t xBuffer = pos.x(); xBuffer < bufferWidth; ++xBuffer, ++xCursor  )
        {
            if(xBuffer < 0)
                continue;

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
