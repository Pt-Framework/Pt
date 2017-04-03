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
#include "PaintSurfaceImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/System/Clock.h>
#include <algorithm>
#include "PixmapSurfaceImpl.h"
#include <cmath>

namespace Pt {

namespace Hmi {
  
ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _frameBuffer( app.frameBuffer() )
, _cursorPos( 0,0 )
, _dpi(96.0)
, _drawCursor(true)
{
    _surface.pixmapImpl()->resize(_frameBuffer.size(), _frameBuffer.strideInBytes() );

    Painter painter(_surface);

    Gfx::RectF rect( Gfx::PointF(0, 0), _surface.size() );
    Pt::Gfx::Color bgColor(65535 * 0.4f, 65535 * 0.3f, 65535 * 0.4f);
    painter.setBrush( Gfx::Brush(bgColor) );
    painter.fillRect(rect);

    updateScreen( Gfx::Rect( Gfx::Point(0,0), _frameBuffer.size()));
}


ScreenImpl::~ScreenImpl()
{
}


void ScreenImpl::init(WindowBase& w)
{
    _windowManager.init(w);
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
    if( ! _cursorBackground.empty() )
    {
        bitBlit( _cursorBackground.data(), 
                 _cursorBackground.width(), 
                 _cursorBackground.height(), 
                 _cursorPos, 
                 image().data(), CopyOp );
    }

    Painter painter(_surface);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);
    painter.setBrush( Pt::Gfx::Color(65535*0.4f, 65535*0.3f, 65535*0.4f) );
    painter.fillRect(updateRect);

    _windowManager.paint(_surface, updateRect);

 //   std::clog << "screen update2: " << clock.stop().toUSecs() << " usecs." << std::endl;
  //  std::clog << "update area2 " << updateRect.topLeft().x() << ',' << updateRect.topLeft().y()
  //            << ' ' << updateRect.width() << 'x' << updateRect.height() << std::endl;
                  
    updateScreen( Gfx::round(updateRect) );
}


Gfx::PointF ScreenImpl::screenPosition(const Gfx::PointF& posRaw)
{     
    const Gfx::SizeF& screenSize = size();
    const double touchWidth  = 800;
    const double touchHeight = 480;

    Gfx::PointF pos = posRaw;

    switch( _frameBuffer.rotation() )
    {
        case FrameBuffer::Rotation0Degree:
        {
            double scaleX =  screenSize.width() / touchWidth;
            double scaleY =  screenSize.height() / touchHeight;
            pos.setX( std::floor(scaleX * posRaw.x()) );
            pos.setY( std::floor(scaleY * posRaw.y()) );
            break;
        }
        
        case FrameBuffer::Rotation90Degree:
        {
              double scaleX =  screenSize.width() / touchHeight;
              double scaleY =  screenSize.height() / touchWidth;
              pos.setX( std::floor((touchHeight - posRaw.y()) * scaleX) );
              pos.setY( std::floor(posRaw.x() * scaleY) );
              break;
        }
    }

    return pos;
}


void ScreenImpl::drawCursor(const Pt::Hmi::MouseEvent& mev)
{
    _drawCursor = true;

    if( ! _cursorBackground.empty() )
    {
        bitBlit( _cursorBackground.data(),  _cursorBackground.width(), _cursorBackground.height(), 
                 _cursorPos, (Pt::uint8_t*)image().data(), CopyOp );

       _frameBuffer.output( image().data(),Gfx::Rect( _cursorPos, _cursorBackground.size() ) );
    }

    const Cursor& cursor = Application::instance().impl()->cursor();

    if( cursor.width() != 0 )
        _cursorPos = Gfx::Point( mev.x() - cursor.xHotspot(), 
                                 mev.y() - cursor.yHotspot() );

    if( _drawCursor )
    {
      if( ! cursor.empty() )
        updateScreen( Gfx::Rect(_cursorPos, 
                                Gfx::Size(cursor.width(), 
                                          cursor.height())) );
    }
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


void ScreenImpl::onKeyEvent(const Pt::Hmi::KeyEvent& ev)
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
    _windowManager.onResize(w, s);
}


void ScreenImpl::onMove(Window& w, const Gfx::PointF& pos)
{
    _windowManager.onMove(w, pos);
}


void ScreenImpl::onFrameChanged(Window& w)
{
    _windowManager.onFrameChanged(w);
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
    _windowManager.onClose(w);
}


void ScreenImpl::onShow(Window& w, bool visible)
{
    _windowManager.onShow(w, visible);
}


void ScreenImpl::onActivate(Window& w)
{ 
    _windowManager.onActivate(&w);
}


void ScreenImpl::onEnable(Window& w, bool enable)
{
    _windowManager.onEnable(w, enable);
}


void ScreenImpl::grabImage( const Pt::uint8_t* buffer, const Gfx::Point& pos,Gfx::Image& image)
{    
    const size_t pixelSizeInByte = _frameBuffer.pixelSize();
    const Gfx::Size& imageSize = image.size();
    const size_t yMax = std::min<size_t>(pos.y() + imageSize.height(), _frameBuffer.height() );    

    size_t widthInPixel = (pos.x() + imageSize.width()) < _frameBuffer.width() ? imageSize.width() 
                                                                               : _frameBuffer.width() - pos.x();
    const size_t widthInByte = widthInPixel * pixelSizeInByte;
    
    for( Pt::ssize_t y = pos.y(); y < yMax; ++y )
    {
        size_t lineOffset = y * _frameBuffer.lineLength() + 
                            pos.x() * pixelSizeInByte;
       
       Pt::uint8_t* pdata = image.data() + (y - pos.y()) * image.view().stride();
        memcpy( pdata, &buffer[lineOffset], widthInByte );
    }
}


void ScreenImpl::drawCursor(Pt::uint8_t* buffer)
{
    const Cursor& cursor = Application::instance().impl()->cursor();

    if( cursor.width() == 0  || cursor.height() == 0 )
        return;

    if( _cursorBackground.width() != cursor.width() || 
        _cursorBackground.height() != cursor.height() )
    {
        Gfx::Size size( cursor.width(), cursor.height() );
        _cursorBackground.reset(_frameBuffer.format(), size); 
    }
    
    grabImage( buffer, _cursorPos, _cursorBackground );
    
    bitBlit(&cursor.andRgb888()[0], cursor.width(), cursor.height(), _cursorPos, buffer, AndOp);
    bitBlit(&cursor.xorRgb888()[0], cursor.width(), cursor.height(), _cursorPos, buffer, XorOp);    
}


void ScreenImpl::updateScreen(const Gfx::Rect& r)
{
    _drawCursor = false;
    drawCursor( image().data() );
    _frameBuffer.output( image().data(), r );
}


void ScreenImpl::bitBlit( const Pt::uint8_t* plane, size_t w, size_t h, 
                          const Gfx::Point& pos, Pt::uint8_t* buffer, BlitOp op )
{
    static const size_t planePixelSize = 4;
    const size_t bufferPixelSize = _frameBuffer.depth() / 8;
    const size_t bufferWidth  = std::min<size_t>( pos.x() + w, _frameBuffer.width() ); 
    const size_t bufferHeight = std::min<size_t>( pos.y() + h, _frameBuffer.height() ); 
    size_t yCursor = 0;
    size_t xCursor = 0;    

    for( size_t yBuffer = pos.y(); yBuffer < bufferHeight; ++yBuffer, ++yCursor )
    {
        const size_t lineOffsetBuffer  = yBuffer * _frameBuffer.lineLength();
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
