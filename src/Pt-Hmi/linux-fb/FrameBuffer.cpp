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
#include "FrameBuffer.h"
#include <Pt/Hmi/Application.h>
#include "ScreenImpl.h"
#include <fcntl.h>
#include <sys/ioctl.h> 
#include <sys/mman.h>

namespace Pt {
namespace Hmi {


  /*_fixedInfo.type;   // 0 -> Packed pixels
                         // 1 -> Non interleaved planes
                         // 2 -> Interleaved planes
                         // 3 -> Text/attributes
                         // 4 -> EGA/VGA planes

    /_fixedInfo.visual; // 0 -> Mono (1=black, 0=white)
                         // 1 -> Mono (1=white, 0=black)
                         // 2 -> True color
                         // 3 -> Pseudo color (like atari)
                         // 4 -> Direct color
                         // 5 -> Pseudo color readonly
*/

FrameBuffer::FrameBuffer()
{  		 
	_fd = open ("/dev/fb0", O_RDWR);

	if(_fd < 0)
		throw std::runtime_error("Could not open framebuffer device" + PT_SOURCEINFO);

	if( 0 > ioctl(_fd, FBIOGET_VSCREENINFO, &_screenInfo) )
		throw std::runtime_error("FBIOGET_VSCREENINFO failed" + PT_SOURCEINFO);


  _screenInfo.yres_virtual = _screenInfo.yres * 2;

  if( 0 > ioctl(_fd, FBIOPUT_VSCREENINFO, &_screenInfo) )
		throw std::runtime_error("FBIOPUT_VSCREENINFO  failed" + PT_SOURCEINFO);

	// Get the fixed state
	if( ioctl(_fd, FBIOGET_VSCREENINFO, &_fixedInfo) < 0 )
		throw std::runtime_error("FBIOGET_FSCREENINFO failed" + PT_SOURCEINFO);
    
	// Memory map the display
	_bufferSize         = _fixedInfo.line_length * _screenInfo.yres;
  _backBufferOffset   = _screenInfo.yres;
	_buffer             =  mmap(NULL, _bufferSize * 2, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);	
}

  

FrameBuffer::~FrameBuffer()
{
	if(_buffer)
		munmap(_buffer, (_fixedInfo.line_length * _screenInfo.yres));

	if(_fd > 0)
		::close(_fd);
} 


void FrameBuffer::bitBlit( const Ui::Image& image )
{
  const size_t imageSize = (image.width() * image.format().pixelSize() + image.stride()) * image.height();

  memcpy( buffer() , image.pixel( 0,0 ), std::min( size(), imageSize ) );  
}


void FrameBuffer::bitBlit( const Pt::uint8_t* plane, size_t w, size_t h, const Ui::Point& pos, BlitOp op )
{
	static const size_t planePixelSize = 4;
	const size_t bufferPixelSize = depth() / 8;
	const size_t bufferWidth  = std::min<size_t>(  pos.x() + w, width() ); 
  const size_t bufferHeight = std::min<size_t>(  pos.y() + h, height() ); 
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


void FrameBuffer::grabImage( Ui::Image& image, const Ui::Point& pos,  const Ui::Size& size)
{
  const size_t pixelSizeInByte = depth() / 8;		

  const size_t yMax = std::min<size_t>(pos.y() + size.height(), height() );	
	const size_t widthInPixel = ((pos.x() + size.width())  < width() ?  size.width() : ( width()  - pos.x() ) );
	const size_t widthInByte = widthInPixel * pixelSizeInByte;	

	const Pt::uint8_t* bufferData = ( const Pt::uint8_t* ) _buffer;
	
	image.resize(  size.width() * size.height(), 0 );	
	
	for( size_t y = pos.y(); y < yMax; ++y )
	{
		const size_t lineOffsetBuffer  = y * _fixedInfo.line_length  + (pos.x() * pixelSizeInByte);
		memcpy( image.pixel(0,y), &bufferData[lineOffsetBuffer], widthInByte );
	}
}



void FrameBuffer::sync()
{
  struct fb_var_screeninfo variable_info;

	ioctl( _fd, FBIOGET_VSCREENINFO, &variable_info ); 	
  
  variable_info.yoffset = _backBufferOffset;

  ioctl( _fd, FBIOPAN_DISPLAY, &variable_info );

  _backBufferOffset =  _backBufferOffset == 0 ? _screenInfo.yres: 0;
}

}} // namespace

