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
#include "PaintSurfaceImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/System/Clock.h>
#include <algorithm>

namespace Pt {

namespace Hmi {
  
ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _frameBuffer( app.frameBuffer() )
, _cursorPos( 0,0 )
, _dpi(96.0)
, _drawCursor(true)
{
    app.eventReady() += Pt::slot( *this, &ScreenImpl::onPointerEvent );
    app.eventReady() += Pt::slot( *this, &ScreenImpl::onKeyEvent );

    _surface.pixmapImpl()->resize(_frameBuffer.size(), _frameBuffer.strideInBytes() );

    Painter painter(_surface);
    painter.clear( Pt::Gfx::Color(0.4f, 0.3f, 0.4f) );

    setCursor(0);
}


ScreenImpl::~ScreenImpl()
{
}


void ScreenImpl::registerWindow(Window& w)
{
    _windowManager.add(w);
}


void ScreenImpl::unregisterWindow(Window& w)
{
    _windowManager.remove(w);
}


void ScreenImpl::setCursor(const Hmi::Cursor* crs)
{        
    _cursor  = crs == 0 ? Hmi::Cursor::defaultCursor() : *crs;        
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
    Pt::System::Clock clock;
    clock.start();   

    //if( ! _cursorBackground.empty() )
    //    bitBlit( _cursorBackground.pixel(0,0), 
    //             _cursorBackground.width(), 
    //             _cursorBackground.height(), 
    //             _cursorPos, 
    //             image().pixel(0,0), CopyOp );

    Painter painter(_surface);
    painter.setBrush( Pt::Gfx::Color(0.4f, 0.3f, 0.4f) );
    painter.fillRect(updateRect);

    _windowManager.paint(_surface, updateRect);
    
    updateScreen();
    
    std::clog << "screen update: " << clock.stop().toUSecs() << " usecs." << std::endl;
    std::clog << "update area " << updateRect.topLeft().x() << ',' << updateRect.topLeft().y()
              << ' ' << updateRect.width() << 'x' << updateRect.height() << std::endl;
}


void ScreenImpl::onPointerEvent( const Pt::Hmi::MouseEvent& mouseEvent )
{        
    _drawCursor =  true;

    if( ! _cursorBackground.empty() )
        bitBlit( _cursorBackground.pixel(0,0), 
                 _cursorBackground.width(), _cursorBackground.height(), 
                 _cursorPos, (Pt::uint8_t*)image().pixel(0,0), CopyOp );

    if( _cursor.width() != 0 )
        _cursorPos = Gfx::Point( mouseEvent.x() - _cursor.xHotspot(), 
                                 mouseEvent.y() - _cursor.yHotspot());

    const Visual* mouseGrabber = Application::instance().impl()->mouseGrabber();
    if(mouseGrabber)
    {
        Pt::Hmi::MouseEvent mev = mouseEvent;

        Gfx::PointF pos = mouseGrabber->fromScreen( mouseEvent.position() );
        mev.setX( pos.x() );
        mev.setY( pos.y() ); 
        mev.setId( mouseGrabber->vid() );

        Application::instance().loop().commitEvent(mev);
    }
    else
    {
        _windowManager.mouseEvent( mouseEvent );        
    }

    if( _drawCursor )
        updateScreen();
}


void ScreenImpl::onKeyEvent(const Pt::Hmi::KeyEvent& ev)
{
    _windowManager.keyEvent(ev);
}


void ScreenImpl::onResize(Window& w, const Gfx::SizeF& s)
{
    windowManager().onResize(w, s);
}


void ScreenImpl::onMove(Window& w, const Gfx::PointF& pos)
{
    windowManager().onMove(w, pos);
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
    windowManager().onShow(w, visible);
}


void ScreenImpl::onActivate(Window& w)
{ 
    windowManager().onActivate(&w);
}


void ScreenImpl::onEnable(Window& w, bool enable)
{
    windowManager().onEnable(w, enable);
}


void ScreenImpl::grabImage( const Pt::uint8_t* buffer, const Gfx::Point& pos,Gfx::Image& image)
{    
    const size_t pixelSizeInByte = _frameBuffer.depth() / 8;        
    const Gfx::Size& size= image.size();
    const size_t yMax = std::min<size_t>(pos.y() + size.height(), height() );    
    const size_t widthInPixel = ((pos.x() + size.width())  < width() ?  size.width() : ( width()  - pos.x() ) );
    const size_t widthInByte = widthInPixel * pixelSizeInByte;            
    
    for( size_t y = pos.y(); y < yMax; ++y )
    {
        size_t lineOffset = y * _frameBuffer.lineLength() + 
                            pos.x() * pixelSizeInByte;
        memcpy( image.pixel(0,y - pos.y()), &buffer[lineOffset], widthInByte );
    }
}


void ScreenImpl::drawCursor(Pt::uint8_t* buffer)
{
    if( _cursor.width() == 0  || _cursor.height() == 0 )
        return;

    if( _cursorBackground.width() != _cursor.width()  || 
        _cursorBackground.height() != _cursor.height() )
    {
        Gfx::Size size(_cursor.width(), _cursor.height());
        _cursorBackground.resize( size, _frameBuffer.format() ); 
    }
    
    grabImage( buffer, _cursorPos, _cursorBackground );
    
    bitBlit(&_cursor.andRgb888()[0], _cursor.width(), _cursor.height(), _cursorPos, buffer, AndOp);
    bitBlit(&_cursor.xorRgb888()[0], _cursor.width(), _cursor.height(), _cursorPos, buffer, XorOp);    
}


void ScreenImpl::updateScreen()
{
    _drawCursor = false;
    drawCursor( image().pixel(0, 0) );
    memcpy( _frameBuffer.buffer(), image().pixel(0,0), _frameBuffer.bufferSize() );            
}


void ScreenImpl::bitBlit(const Gfx::Image& image, Pt::uint8_t* buffer)
{
  size_t bytesPerLine = image.width() * image.format().pixelSize() + image.stride();
  size_t imageSize = bytesPerLine * image.height();

  imageSize = std::min(_frameBuffer.bufferSize(), imageSize);

  memcpy(buffer, image.pixel(0 ,0), imageSize );  
}


void ScreenImpl::bitBlit( const Pt::uint8_t* plane, size_t w, size_t h, 
                          const Gfx::Point& pos, Pt::uint8_t* buffer, BlitOp op )
{
    static const size_t planePixelSize = 4;
    const size_t bufferPixelSize = _frameBuffer.depth() / 8;
    const size_t bufferWidth  = std::min<size_t>(  pos.x() + w, width() ); 
    const size_t bufferHeight = std::min<size_t>(  pos.y() + h, height() ); 
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
