/* Copyright (C) 2015 Marc Boris Duerner 
 * Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include "ScreenImpl.h"
#include <Pt/Hmi/PaintSurface.h>
#include "PaintSurfaceImpl.h"
#include <algorithm>
#include <fcntl.h>
#include <sys/ioctl.h> 
#include <sys/mman.h>
#include <unistd.h>

namespace Pt{
namespace Hmi{

ScreenImpl::ScreenImpl()
{
 // Open the frame buffer device
    _fd = open ("/dev/fb0", O_RDWR);

    if(_fd < 0)
        throw std::runtime_error("Could not open framebuffer device" + PT_SOURCEINFO);

/*
    if( 0 > ioctl(_fd, FBIOGET_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOGET_VSCREENINFO failed" + PT_SOURCEINFO);

    _screenInfo.bits_per_pixel = 16;
    _screenInfo.xres           = 640;
    _screenInfo.yres           = 480;

    if( 0 > ioctl(_fd, FBIOPUT_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOPUT_VSCREENINFO failed" + PT_SOURCEINFO);
*/

    if( 0 > ioctl(_fd, FBIOGET_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOGET_VSCREENINFO failed" + PT_SOURCEINFO);

    // Get the fixed state
    if( ioctl(_fd, FBIOGET_FSCREENINFO, &_fixedInfo) < 0 )
        throw std::runtime_error("FBIOGET_FSCREENINFO failed" + PT_SOURCEINFO);

    //_fixedInfo.type;   // 0 -> Packed pixels
                         // 1 -> Non interleaved planes
                         // 2 -> Interleaved planes
                         // 3 -> Text/attributes
                         // 4 -> EGA/VGA planes

    //_fixedInfo.visual; // 0 -> Mono (1=black, 0=white)
                         // 1 -> Mono (1=white, 0=black)
                         // 2 -> True color
                         // 3 -> Pseudo color (like atari)
                         // 4 -> Direct color
                         // 5 -> Pseudo color readonly

  // Memory map the display
  std::clog<<"LineWidth = " << _fixedInfo.line_length << " xres = " << _screenInfo.xres << std::endl;
  std::clog<<"yres = " << _screenInfo.yres << std::endl;
  const unsigned widthInBytes = _screenInfo.xres * _screenInfo.bits_per_pixel / 8;
  _bufferSize     = _fixedInfo.line_length * _screenInfo.yres;
  _buffer         =  mmap(NULL, _bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);	

	Visible = true;
	Size = Ui::SizeF( _screenInfo.xres,  _screenInfo.yres );
	Position = Ui::PointF(0,0 );
	BackColor = Ui::Color(170,170,170);	
	eventReceived() += Pt::slot( _windowManager, &WindowManager::onKeyInput );
}


void ScreenImpl::bitBlit(const std::vector<Pt::uint8_t>& plane, size_t w, size_t h, const Ui::Point& pos, BlitOp op)
{
	static const size_t planePixelSize = 4;
	const size_t bufferPixelSize = depth() / 8;
	const size_t bufferHeight = std::min<size_t>(  pos.y() + h, _screenInfo.yres ); 
	const size_t bufferWidth  = std::min<size_t>(  pos.x() + w, _screenInfo.xres ); 

	size_t yCursor = 0;
	size_t xCursor = 0;	

	for( size_t yBuffer = pos.y(); yBuffer < bufferHeight; ++yBuffer, ++yCursor )
	{
		const size_t lineOffsetBuffer  = yBuffer * _fixedInfo.line_length;
		const size_t lineOffsetCursor  = yCursor * (w * planePixelSize);
		
		xCursor = 0;

		for( size_t xBuffer = pos.x(); xBuffer < bufferWidth; ++xBuffer, ++xCursor  )
		{			
			Pt::uint8_t* pointerBuffer = &((Pt::uint8_t*)_buffer)[lineOffsetBuffer + (xBuffer * bufferPixelSize)];
			const Pt::uint8_t* pointerCursor = &plane[lineOffsetCursor + (xCursor * planePixelSize)];

			switch( depth() )
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

						case CopyOp:
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


void ScreenImpl::saveCursorBackImage( const Pt::Hmi::PointerEvent& mouseEvent )
{
	const size_t pixelSizeInByte = depth() / 8;		
	_cursorPos    = Ui::Point( mouseEvent.x(), mouseEvent.y() );	
	_cursorWidth  = Cursor.get().width();
	_cursorHeight = Cursor.get().height();
	
	const size_t yMax = std::min<size_t>(_cursorPos.y() + _cursorHeight, _screenInfo.yres );	
	const size_t widthInPixel = ((_cursorPos.x() + _cursorWidth)  < _screenInfo.xres ? _cursorWidth : ( _screenInfo.xres - _cursorPos.x() ) );
	const size_t widthInByte = widthInPixel * pixelSizeInByte;	
	const Pt::uint8_t* bufferData = ( const Pt::uint8_t* ) _buffer;
	
	_cursorBuffer.resize( _cursorWidth * _cursorHeight * pixelSizeInByte, 0 );	
	
	for( size_t y = _cursorPos.y(); y < yMax; ++y )
	{
		const size_t lineOffsetBuffer  = y * _fixedInfo.line_length  + (_cursorPos.x() * pixelSizeInByte);
    const size_t lineOffsetCursor  = ( y - _cursorPos.y() )  * widthInByte;     
		memcpy( &_cursorBuffer[lineOffsetCursor], &bufferData[lineOffsetBuffer], widthInByte );
	}
}


void ScreenImpl::onPointerInput( const Pt::Hmi::PointerEvent& mouseEvent )
{	
	if( _cursorBuffer.size() != 0 )
		bitBlit( _cursorBuffer, _cursorWidth, _cursorHeight, _cursorPos, CopyOp );

	_windowManager.onPointerInput(mouseEvent);	

	_cursorBuffer.clear();

	if( Cursor.get().width() == 0 )
		return;	

	saveCursorBackImage(mouseEvent);

  //Todo: only one blit 
	bitBlit( Cursor.get().andRgb888(), Cursor.get().width(), Cursor.get().height(), Ui::Point( ( int) mouseEvent.x(), (int) mouseEvent.y()), AndOp );
	bitBlit( Cursor.get().xorRgb888(), Cursor.get().width(), Cursor.get().height(), Ui::Point( (int) mouseEvent.x(), (int) mouseEvent.y()), XorOp );
}


void ScreenImpl::onInvalidate()
{	
	render();		
	_windowManager.render();

	const Ui::Image& image= paintSurface().impl()->image();

	memcpy( _buffer, image.pixel(0, 0), _bufferSize);	
}


ScreenImpl::~ScreenImpl()
{
	if(_buffer)
		munmap(_buffer, _bufferSize);

	if(_fd > 0)
		::close(_fd);
}

}}
