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
#include <Pt/System/Logger.h>


PT_LOG_DEFINE("Pt.Hmi.Screen")


namespace Pt{
namespace Hmi{

ScreenImpl::ScreenImpl()
: _windowManager( *this )
{


	std::clog<<"a1"<<std::endl;
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
    unsigned _pitch = _screenInfo.xres * _screenInfo.bits_per_pixel / 8;
    _bufferSize     = _pitch * _screenInfo.yres;
    _buffer         =  mmap(NULL, _bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);	
	Pt::uint8_t* pixel = (	Pt::uint8_t* ) _buffer;
	for( size_t i = 0; i < _bufferSize; ++i)
	{
		pixel[i] = 160;
	}
	

	PT_LOG_INFO("open Screen = " << _screenInfo.xres << ", " << _screenInfo.yres );
	Size = Pt::Gfx::SizeF( _screenInfo.xres,  _screenInfo.yres );
	Position = Pt::Gfx::PointF(0,0 );
	eventReceived() += Pt::slot( *this, &ScreenImpl::onPointerInput );
	eventReceived() += Pt::slot( _windowManager, &WindowManager::onKeyInput );

}


void ScreenImpl::blitPlane(const std::vector<Pt::uint8_t>& plane, size_t w, size_t h, const Gfx::Point& pos, BlitOp op)
{
	static const size_t planePixelSize = 4;
	const size_t bufferPixelSize = depth() / 8;
	const size_t bufferHeight = std::min<size_t>(  pos.y() + h, _screenInfo.yres ); 
	const size_t bufferWidth  = std::min<size_t>(  pos.x() + w, _screenInfo.xres ); 

	size_t yCursor = 0;
	size_t xCursor = 0;
	

	for( size_t yBuffer = pos.y(); yBuffer < bufferHeight; ++yBuffer, ++yCursor )
	{
		const size_t lineOffsetBuffer  = yBuffer * (_screenInfo.xres * bufferPixelSize);
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
					}
				}
				break;

				case 16:
				break;
			}
		}
	}
}


void ScreenImpl::onPointerInput( const Pt::Hmi::PointerEvent& mouseEvent )
{
	
	
	_windowManager.onPointerInput(mouseEvent);	

	if( Cursor.get().width() == 0 )
		return;

	blitPlane( Cursor.get().andRgb888(), Cursor.get().width(), Cursor.get().height(), Pt::Gfx::Point( ( int) mouseEvent.x(), (int) mouseEvent.y()), AndOp );
	blitPlane( Cursor.get().xorRgb888(), Cursor.get().width(), Cursor.get().height(), Pt::Gfx::Point( (int) mouseEvent.x(), (int) mouseEvent.y()), XorOp );
}


void ScreenImpl::copyImageData(ssize_t toX, ssize_t toY, const char* data, size_t fromWidth, size_t fromHeight)
{
	toX  = (_screenInfo.xres  - fromWidth) ;

	size_t pixelSize = depth() / 8;
	unsigned bufferOffset = toX + ( toY * _screenInfo.xres );
	char* bufferData = (char*)( _buffer) + ( bufferOffset * pixelSize);

	for(size_t n = 0; n < fromHeight; ++n)
	{
		memcpy(bufferData, data, fromWidth * pixelSize);
		bufferData += (size_t) _screenInfo.xres * pixelSize;
		data += fromWidth * pixelSize;
	}
}


void ScreenImpl::onInvalidate()
{
	_windowManager.render();
	drawImage( 0, 0, paintSurface().impl()->image().begin(), paintSurface().impl()->image().end(), _screenInfo.xres, _screenInfo.yres );
	
}


ScreenImpl::~ScreenImpl()
{
	if(_buffer)
		munmap(_buffer, _bufferSize);

	if(_fd > 0)
		close(_fd);
}

}}
