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

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Forms/PaintSurface.h>
#include <Pt/Forms/Cursor.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/System/Clock.h>
#include <algorithm>

namespace Pt {

namespace Forms {
  
ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _frameBuffer( app.frameBuffer() )
, _cursorPos( 0,0 )
, _dpi(96.0)
, _drawCursor(true)
{
    app.eventReady() += Pt::slot( *this, &ScreenImpl::onMouseEvent );
    app.eventReady() += Pt::slot( *this, &ScreenImpl::onKeyEvent );

    _surface.pixmapImpl()->resize(_frameBuffer.size(), _frameBuffer.strideInBytes() );

    Painter painter(_surface);
    painter.clear( Pt::Gfx::Color(65535*0.4f, 65535 *0.3f, 65535 *0.4f) );

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


const Gfx::Image& ScreenImpl::image() const
{
    return _surface.pixmapImpl()->image();
}


Gfx::Image& ScreenImpl::image()
{
    return _surface.pixmapImpl()->image();
}


void ScreenImpl::onMouseEvent( const Pt::Forms::MouseEvent& mouseEvent )
{        
    _drawCursor =  true;

    if( !_cursorBackground.empty() )
    {
        bitBlit( _cursorBackground.data(), _cursorBackground.width(), _cursorBackground.height(), 
                 _cursorPos, image().data(), CopyOp );
    }

    if( _cursor.width() != 0 )
        _cursorPos = Gfx::Point( mouseEvent.x() - _cursor.xHotspot() , mouseEvent.y() - _cursor.yHotspot());

    _windowManager.pointerInput( mouseEvent );
    
    if( _drawCursor )
        updateScreen();
}


void ScreenImpl::onKeyEvent(const Pt::Forms::KeyEvent& ev)
{
    _windowManager.keyInput(ev);
}


void ScreenImpl::grabImage( const Pt::uint8_t* buffer, const Gfx::Point& pos,Gfx::Image& image)
{    
    const Gfx::Size& size= image.size();
    const size_t yMax = std::min<size_t>(pos.y() + size.height(), height() );    
    const size_t widthInPixel = ((pos.x() + size.width())  < width() ?  size.width() : ( width()  - pos.x() ) );
    const size_t widthInByte = widthInPixel * _frameBuffer.pixelSize();            
    
    for( size_t y = pos.y(); y < yMax; ++y )
    {
        size_t lineOffset = y * _frameBuffer.lineLength() + 
                            pos.x() * _frameBuffer.pixelSize();

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
    _drawCursor    = false;
    drawCursor( image().data() );
    memcpy( _frameBuffer.buffer(), image().data(), _frameBuffer.bufferSize() );
}


void ScreenImpl::update(const Gfx::RectF& updateRect)
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

    _windowManager.render(_surface, updateRect);
    
    updateScreen();
    std::clog << "screen update: " << clock.stop().toUSecs() << " usecs." << std::endl;
    std::clog << "update area " << updateRect.topLeft().x() << ',' << updateRect.topLeft().y()
              << ' ' << updateRect.width() << 'x' << updateRect.height() << std::endl;
}


void ScreenImpl::setCursor( const Forms::Cursor* crs )
{        
    _cursor = crs == 0 ? Forms::Cursor::defaultCursor() : *crs;        
}


void ScreenImpl::bitBlit( const Pt::uint8_t* plane, size_t w, size_t h, const Gfx::Point& pos, Pt::uint8_t* buffer, BlitOp op )
{    
    const size_t bufferWidth  = std::min<size_t>(  pos.x() + w, width() ); 
    const size_t bufferHeight = std::min<size_t>(  pos.y() + h, height() ); 
    size_t yCursor = 0;
    size_t xCursor = 0;    

    for( size_t yBuffer = pos.y(); yBuffer < bufferHeight; ++yBuffer, ++yCursor )
    {
        const size_t lineOffsetBuffer  = yBuffer * _frameBuffer.lineLength();
        const size_t lineOffsetCursor  = yCursor * (w * _frameBuffer.pixelSize());
        
        xCursor = 0;

        for( size_t xBuffer = pos.x(); xBuffer < bufferWidth; ++xBuffer, ++xCursor  )
        {            
            Pt::uint8_t* pointerBuffer = &((Pt::uint8_t*)buffer)[lineOffsetBuffer + (xBuffer * _frameBuffer.pixelSize())];
            const Pt::uint8_t* pointerCursor = &plane[lineOffsetCursor + (xCursor * _frameBuffer.pixelSize())];

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
                {
                    Pt::uint16_t* pixelBuffer = (Pt::uint16_t*) pointerBuffer;
                    const Pt::uint16_t* pixelCursor = (const Pt::uint16_t*) pointerCursor;

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
                }
                break;
            }
        }
    }
}


void ScreenImpl::onActivate()
{ 
}

} // namespace

} // namespace
