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
  produce an executable, this file does not by itself cause th93e
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

#include "FrameBuffer.h"
#include <Pt/Hmi/Application.h>
#include "ScreenImpl.h"
#include <fcntl.h>
#include <sys/ioctl.h> 
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>

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
: _rotation(Rotation90Degree)
{           
    _fd = open ("/dev/fb0", O_RDWR);

    if(_fd < 0)
        throw std::runtime_error("Could not open framebuffer device" + PT_SOURCEINFO);

    if( 0 > ioctl(_fd, FBIOGET_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOGET_VSCREENINFO failed" + PT_SOURCEINFO);

    // Get the fixed state
    if( ioctl(_fd, FBIOGET_FSCREENINFO, &_fixedInfo) < 0 )
        throw std::runtime_error("FBIOGET_FSCREENINFO failed" + PT_SOURCEINFO);

    // Memory map the display
    _bufferSize = _fixedInfo.line_length * _screenInfo.yres;
    _rotationBuffer.resize(_bufferSize);
    _yoffset    = _screenInfo.yres;
    _buffer     = (char*)  mmap(NULL, _bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
    size_t stride = _fixedInfo.line_length - ( _screenInfo.xres *  depth() /8  );
    const size_t noOfBytesPerPixel = depth() % 8 != 0 ? depth() / 8 + 1 : depth() / 8;

    switch( noOfBytesPerPixel )
    {
        case 2:
            _format = new Gfx::Rgb565Format();
            break;

        case 3:
            _format = new Gfx::Rgb888Format();
            break;

        case 4:
            _format =  new Gfx::Argb8888Format();
            break;

        default:
            _format =  new Gfx::Argb8888Format();
            break;
    }

    std::clog<<"Sreen HW resolution (" <<_screenInfo.xres<< "," <<_screenInfo.yres << ") Pixel size = "<<_format->pixelSize()<< " Stride = "<<stride<<std::endl;
    std::clog<<"Sreen VR resolution (" <<width()<< "," <<height() << ") Pixel size = "<<_format->pixelSize()<< " Stride = "<<strideInBytes()<<std::endl;
}


FrameBuffer::~FrameBuffer()
{
    if(_buffer)
        munmap(_buffer, (_fixedInfo.line_length * _screenInfo.yres));

    if(_fd > 0)
        ::close(_fd);

  if( _format != 0)
     delete _format;
} 

size_t FrameBuffer::width() const
{
    switch( _rotation)
    {
      default:
        return _screenInfo.xres;

      case  Rotation90Degree:
      case  Rotation270Degree:
        return _screenInfo.yres; 
    }

    return 0;
}

size_t FrameBuffer::lineLength() const
{
    switch( _rotation)
    {
      case  Rotation90Degree:
      case  Rotation270Degree:
        return width() * _format->pixelSize();
    }

    return _fixedInfo.line_length;
}

size_t FrameBuffer::strideInBytes() const
{
    switch( _rotation)
    {
      case  Rotation90Degree:
      case  Rotation270Degree:
        return 0;
    }

    return  _fixedInfo.line_length - ( _screenInfo.xres *  depth() /8  );
}

size_t FrameBuffer::bufferSize() const
{
    switch( _rotation)
    {
      case  Rotation90Degree:
      case  Rotation270Degree:
        return width()* height() * _format->pixelSize();
    }

    return _bufferSize;
}

size_t FrameBuffer::height() const
{
    switch( _rotation)
    {
      default:
        return _screenInfo.yres;

      case  Rotation90Degree:
      case  Rotation270Degree:
        return _screenInfo.xres; 
    }

    return 0;
}

void FrameBuffer::set( const Pt::uint8_t* frame )
{
    switch( _rotation)
    {
      case Rotation0Degree:
          memcpy( _buffer, frame, _bufferSize );
      break;

      case Rotation90Degree:
        for( size_t w = 0; w < width(); ++w)
        {
           for( size_t h = 0; h < height(); ++h)
           {
              int* dest = (int*)pixelBuffer( h, _screenInfo.yres - w -1);

              const int* src = (int*)pixelFrame(frame, w, h);
              *dest =  *src;
            }
        }

        memcpy( _buffer, &_rotationBuffer[0], _rotationBuffer.size());
      break;

      case Rotation180Degree:
        throw std::logic_error("Not implemented");
      break;

      case Rotation270Degree:
        throw std::logic_error("Not implemented");
      break;
    }
}


} // namespace

} // namespace
