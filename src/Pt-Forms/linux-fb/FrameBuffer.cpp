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
#include "ScreenImpl.h"

#include <Pt/Forms/Application.h>
#include <Pt/Gfx/Rgb16Format.h>
#include <Pt/Gfx/Rgb32Format.h>
#include <Pt/Gfx/Argb32Format.h>

#include <sstream>

#include <fcntl.h>
#include <sys/ioctl.h> 
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>

namespace Pt {

namespace Forms {

/*
    _fixedInfo.type;   // 0 -> Packed pixels
                       // 1 -> Non interleaved planes
                       // 2 -> Interleaved planes
                       // 3 -> Text/attributes
                       // 4 -> EGA/VGA planes

    _fixedInfo.visual; // 0 -> Mono (1=black, 0=white)
                       // 1 -> Mono (1=white, 0=black)
                       // 2 -> True color
                       // 3 -> Pseudo color (like atari)
                       // 4 -> Direct color
                       // 5 -> Pseudo color readonly
*/

FrameBuffer::FrameBuffer()
: _fd(-1)
, _rotation(Rotate0)
, _bufferSize(0)
, _buffer(0)
, _format(0)
, _lineSize(0)
, _pixelSize(0)
{           
    std::string fbdev = Pt::System::Application::getEnvVar("PT_FRAMEBUFFER_DEVICE");
    if( fbdev.empty() )
      fbdev = "/dev/fb0";

    _fd = open(fbdev.c_str(), O_RDWR);
    if(_fd < 0)
        throw std::runtime_error("invalid framebuffer device");

    if( 0 > ioctl(_fd, FBIOGET_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOGET_VSCREENINFO failed");

    if( 0 > ioctl(_fd, FBIOGET_FSCREENINFO, &_fixedInfo) )
        throw std::runtime_error("FBIOGET_FSCREENINFO failed");

    _bufferSize = _fixedInfo.line_length * _screenInfo.yres;
    _buffer     = (char*) mmap(NULL, _bufferSize, PROT_READ|PROT_WRITE, MAP_SHARED, _fd, 0);

    _lineSize = _fixedInfo.line_length;

    switch(_screenInfo.bits_per_pixel)
    {
        case 15:
            throw std::runtime_error("RGB 565 not supported");

        case 16:
            _format = new Gfx::Rgb16Format();
            _pixelSize = 2;
            break;

        default:
        case 24:
        case 32:
            _format =  new Gfx::Argb32Format();
            _pixelSize = 4;
            break;
    }

    std::clog << "Screen info: " << _screenInfo.xres << "x" << _screenInfo.yres
              << ", pixel size: " << _pixelSize << ", stride: " << strideSize()
              << ", depth: " << _screenInfo.bits_per_pixel
              << ", buffer size: " << _bufferSize << std::endl;

    int rotval = 0;
    std::string value = Pt::System::Application::getEnvVar("PT_FRAMEBUFFER_ROTATE");
    std::istringstream iss(value);
    iss >> rotval;
    
    switch(rotval)
    {
        default:
        case 0:
          setRotation(Rotate0);
          break;

        case 1:
          setRotation(Rotate90);
          break;
    }
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


void FrameBuffer::setRotation(Rotation r)
{
  _rotation = r;

  switch(_rotation)
  {
    default:
    case Rotate0:
        _lineSize = _fixedInfo.line_length;
        break;

    case  Rotate90:
        _lineSize =  _screenInfo.yres * _pixelSize;
        break;
  }

  std::clog << "Screen rotation: " << width() << "x" << height() 
            << ", pixel size: " << _pixelSize 
            << ", stride: " << strideSize() << std::endl;
}


size_t FrameBuffer::width() const
{
    switch( _rotation)
    {
        default:
        case Rotate0:
            return _screenInfo.xres;

        case  Rotate90:
            return _screenInfo.yres; 
    }

    return 0;
}


size_t FrameBuffer::height() const
{
    switch( _rotation)
    {
        default:
            case Rotate0:
            return _screenInfo.yres;

        case Rotate90:
            return _screenInfo.xres; 
    }

    return 0;
}


size_t FrameBuffer::strideSize() const
{
    switch(_rotation)
    {
        default:
        case Rotate0:
            return _fixedInfo.line_length - (_screenInfo.xres * _screenInfo.bits_per_pixel / 8);
            break;

        case Rotate90:
            return 0;
    }

    return 0;
}


void FrameBuffer::output( const Pt::uint8_t* frame, const Rect& areaIn )
{
    switch( _rotation)
    {
      case Rotate0:
      {
        const Rect clipArea = areaIn.intersect(Rect( Point(0, 0), size() ));
        const int clipRight  = clipArea.x() + clipArea.width();
        const int clipBottom = clipArea.y() + clipArea.height();
        const int widthInByte = clipArea.width()*_pixelSize;

        for (Pt::ssize_t h = clipArea.y(); h < clipBottom; ++h)
        {
            Pt::uint32_t* dest = (Pt::uint32_t*)pixelFB(clipArea.x(), h);
            const  Pt::uint32_t* src = (Pt::uint32_t*)pixelFrame(frame, clipArea.x(), h);
            memcpy(dest, src, widthInByte);
        }
      }
      break;

      case Rotate90:
      {
        const Rect clipArea = areaIn.intersect( Rect(Point(0,0), size()) );
        const int clipRight = clipArea.x() + clipArea.width();
        const int clipBottom = clipArea.y() + clipArea.height();
        const Pt::ssize_t height = _screenInfo.yres - 1; 

        for( Pt::ssize_t w = clipArea.x(); w < clipRight; ++w)
        {
           const Pt::ssize_t yPos =  height - w;

           for(  Pt::ssize_t h = clipArea.y(); h < clipBottom; ++h)
           {
              Pt::uint32_t* dest = ( Pt::uint32_t*)pixelFB( h, yPos);

              const  Pt::uint32_t* src = ( Pt::uint32_t*)pixelFrame(frame, w, h);
              *dest =  *src;
            }
        }
        break;
      }
    }
}

} // namespace

} // namespace
