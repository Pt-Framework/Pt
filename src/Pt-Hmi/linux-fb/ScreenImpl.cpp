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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include "ScreenImpl.h"
#include "FrameBuffer.h"
#include "ApplicationImpl.h"
#include "PaintSurfaceImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/System/Clock.h>
#include <algorithm>

namespace Pt {

namespace Hmi {
  
ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _frameBuffer( app.frameBuffer() )
, _cursorPos( 0,0 )
, _image( _frameBuffer.size(), _frameBuffer.format(), _frameBuffer.strideInBytes() )
, _dpi(96.0)
, _drawCursor( true)
{
	app.eventReady() += Pt::slot(_eventReceived);

	Size.set( Gfx::SizeF(_frameBuffer.width(), _frameBuffer.height()) );
	BackColor.set( Gfx::Color(170/255.0f, 170/255.0f, 170/255.0f) );
	Visible.set(true);			
	setCursor(0);	
	
  eventReceived() += Pt::slot( *this, &ScreenImpl::onPointerInput );
}


ScreenImpl::~ScreenImpl()
{
}


void ScreenImpl::onPointerInput( const Pt::Hmi::PointerEvent& mouseEvent )
{		
	_drawCursor =  true;

	if( !_cursorBackground.empty() )
		bitBlit( _cursorBackground.pixel(0,0), _cursorBackground.width(), _cursorBackground.height(), _cursorPos, (Pt::uint8_t*)  _image.pixel(0,0), CopyOp );

	if( Cursor.get().width() != 0 )
		_cursorPos = Gfx::Point( mouseEvent.x() - Cursor.get().xHotspot() , mouseEvent.y() - Cursor.get().yHotspot());

	_windowManager.pointerInput( mouseEvent );		
	
	if( _drawCursor )
		updateScreen();
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
		const size_t lineOffsetBuffer  = y * _frameBuffer.lineLength() + (pos.x() * pixelSizeInByte);
		memcpy( image.pixel(0,y - pos.y()), &buffer[lineOffsetBuffer], widthInByte );
	}
}


void ScreenImpl::drawCursor( Pt::uint8_t* buffer )
{
	//Draw the mouse to image.
	if( Cursor.get().width() == 0  || Cursor.get().height() == 0 )
		return;

	if( (_cursorBackground.width() != Cursor.get().width())  || (_cursorBackground.height() !=  Cursor.get().height()) )
		_cursorBackground.resize(Gfx::Size( Cursor.get().width(),Cursor.get().height()), _frameBuffer.format() ); 

	grabImage( buffer, _cursorPos, _cursorBackground );
	
	bitBlit( &Cursor.get().andRgb888()[0], Cursor.get().width(), Cursor.get().height(), _cursorPos, buffer, AndOp );
	bitBlit( &Cursor.get().xorRgb888()[0], Cursor.get().width(), Cursor.get().height(), _cursorPos, buffer, XorOp );    
}


void ScreenImpl::updateScreen()
{
	_drawCursor	= false;
	drawCursor(  _image.pixel(0,0) );
	memcpy( _frameBuffer.buffer(), _image.pixel(0,0), _frameBuffer.bufferSize() );			
}

void ScreenImpl::onInvalidate()
{		
	//Load the render pipeline
	Window::render();
	
	//Draw the render pipeline to image
	Hmi::Painter& painter = surface().painter(); 
	painter.flush();
	updateScreen();
}


void ScreenImpl::setCursor( const Hmi::Cursor* cursor )
{		
	Cursor = (cursor == 0 ? Hmi::Cursor::defaultCursor() : *cursor );		
}


void ScreenImpl::bitBlit( const Gfx::Image& image, Pt::uint8_t* buffer )
{
  const size_t imageSize = (image.width() * image.format().pixelSize() + image.stride()) * image.height();

  memcpy( buffer , image.pixel( 0,0 ), std::min( _frameBuffer.bufferSize(), imageSize ) );  
}


void ScreenImpl::bitBlit( const Pt::uint8_t* plane, size_t w, size_t h, const Gfx::Point& pos, Pt::uint8_t* buffer, BlitOp op )
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
				}
				break;

				case 16:
          //TODO:
				break;
			}
		}
	}  
}
}}
