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
#include <Pt/Gfx/Rgb16.h>
#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/Argb32.h>

#include <sstream>

#if defined(PT_FORMS_WITH_CPU_NEON)
#include <arm_neon.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#endif

#include <fcntl.h>
#include <sys/ioctl.h> 
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>

namespace {

#if defined(PT_FORMS_WITH_CPU_NEON)
 
void rotate90_argb32(const uint8_t* __restrict src, int src_width, int /*src_height*/,
                     int src_rect_x, int src_rect_y, int src_rect_w, int src_rect_h,
                     uint8_t* __restrict dst, int dst_stride, int dst_x0, int dst_y0)
{
    const int TS = 4;
   
    const int pixel_size = 4;
    const int src_stride = src_width * pixel_size;

    for (int ty = 0; ty < src_rect_h; ty += TS)
    {
        for (int tx = 0; tx < src_rect_w; tx += TS)
        {
            int bh = std::min(TS, src_rect_h - ty);
            int bw = std::min(TS, src_rect_w - tx);
 
            int src_abs_y = src_rect_y + ty;
            int src_abs_x = src_rect_x + tx;
 
            // use scalar if not 4x4 block
            if (bh < TS || bw < TS)
            {
                for (int y = 0; y < bh; ++y)
                {
                    const uint8_t* srow = src + (src_abs_y + y) * src_stride + src_abs_x * 4;
 
                    for (int x = 0; x < bw; ++x)
                    {
                        int dst_x = dst_x0 + (ty + y);
                        int dst_y = dst_y0 + (src_rect_w - 1 - (tx + x));
 
                        uint32_t pixel = *(const uint32_t*)(srow + x * 4);
                        *(uint32_t*)(dst + dst_y * dst_stride + dst_x * 4) = pixel;
                    }
                }
                continue;
            }
 
            // use neon for 4x4 blocks
            const uint32_t* src_ptr = (const uint32_t*)(src + (src_abs_y * src_stride + src_abs_x * 4));
 
            uint32x4_t r0 = vld1q_u32(src_ptr + 0 * src_stride / 4);
            uint32x4_t r1 = vld1q_u32(src_ptr + 1 * src_stride / 4);
            uint32x4_t r2 = vld1q_u32(src_ptr + 2 * src_stride / 4);
            uint32x4_t r3 = vld1q_u32(src_ptr + 3 * src_stride / 4);
 
            uint32x4x2_t t01 = vtrnq_u32(r0, r1);
            uint32x4x2_t t23 = vtrnq_u32(r2, r3);
 
            uint32x4_t row0 = vcombine_u32(vget_low_u32(t01.val[0]),  vget_low_u32(t23.val[0]));
            uint32x4_t row1 = vcombine_u32(vget_low_u32(t01.val[1]),  vget_low_u32(t23.val[1]));
            uint32x4_t row2 = vcombine_u32(vget_high_u32(t01.val[0]), vget_high_u32(t23.val[0]));
            uint32x4_t row3 = vcombine_u32(vget_high_u32(t01.val[1]), vget_high_u32(t23.val[1]));
 
            uint32_t* dst_ptr = (uint32_t*)(dst + dst_y0 * dst_stride + dst_x0 * 4);
 
            int dst_base_x_pix = ty;
            int dst_base_y_pix = (src_rect_w - 1 - (tx + 3));
 
            vst1q_u32(dst_ptr + (dst_base_y_pix + 0) * (dst_stride / 4) + dst_base_x_pix, row3);
            vst1q_u32(dst_ptr + (dst_base_y_pix + 1) * (dst_stride / 4) + dst_base_x_pix, row2);
            vst1q_u32(dst_ptr + (dst_base_y_pix + 2) * (dst_stride / 4) + dst_base_x_pix, row1);
            vst1q_u32(dst_ptr + (dst_base_y_pix + 3) * (dst_stride / 4) + dst_base_x_pix, row0);
        }
    }
}
 
#else
 
void rotate90_argb32(const Pt::uint8_t* src, int src_width, int src_height,
                     int src_x, int src_y, int width, int height,
                     Pt::uint8_t* dst, int dst_stride, int dst_x, int dst_y)
{
    const int pixel_size = 4;
    const int src_stride = src_width * pixel_size;
 
    const Pt::uint8_t* src_base = src + src_y * src_stride + (src_x + width - 1) * pixel_size;
    Pt::uint8_t*       dst_base = dst + dst_y * dst_stride + dst_x * pixel_size;
 
    for( int w = 0; w < width; ++w)
    {
        const Pt::uint8_t* src_ptr = src_base;
        Pt::uint8_t* dst_ptr = dst_base;
 
        for (int y = 0; y < height; ++y)
        {
            memcpy( dst_ptr, src_ptr, sizeof(Pt::uint32_t) );
            dst_ptr += pixel_size;
            src_ptr += src_stride;
        }
 
        src_base -= pixel_size;
        dst_base += dst_stride;
    }
}
 
#endif

} // namespace

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
            throw std::runtime_error("RGB 555 not supported");

        case 16:
            throw std::runtime_error("RGB 565 not supported");
            //_format = new Gfx::Rgb16Format();
            _pixelSize = 2;
            break;

        default:
        case 24:
        case 32:
            _format =  new Gfx::Rgb32();
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


void FrameBuffer::output( const Pt::uint8_t* frame, const Gfx::RectI& rect )
{
    switch( _rotation)
    {
      case Rotate0:
      {
        const Gfx::RectI clipArea = rect.toIntersected(Gfx::RectI( Gfx::PointI(0, 0), size() ));
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
        const Gfx::RectI clipArea = rect.toIntersected( Gfx::RectI(Gfx::PointI(0,0), size()) );
        const int dst_x = clipArea.y();
        const int dst_y = size().width() - clipArea.x() - clipArea.width();
        const int dst_stride = size().height() * _pixelSize;
 
        // std::clog << "Rotating block: " << clipArea.x() << " " << clipArea.y() << " "
        //                                 << clipArea.width() << " " << clipArea.height()
        //                                 << " -> " << dst_x << " " << dst_y << std::endl;
 
        rotate90_argb32(frame, size().width(), size().height(),
                        clipArea.x(), clipArea.y(), clipArea.width(), clipArea.height(),
                        (Pt::uint8_t*)_buffer, dst_stride, dst_x, dst_y);
      }
      break;
    }
}

} // namespace

} // namespace
