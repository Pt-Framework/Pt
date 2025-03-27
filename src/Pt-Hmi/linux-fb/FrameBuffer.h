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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef Pt_Hmi_FrameBuffer_h
#define Pt_Hmi_FrameBuffer_h

#include "InputDevice.h"
#include <Pt/System/MainLoop.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/Cursor.h>
#include <linux/fb.h>

namespace Pt {

namespace Hmi {

class FrameBuffer
{
  public:
    typedef Gfx::Image::pos_t      pos_t;
    typedef Gfx::BasicPoint<pos_t> Point;
    typedef Gfx::BasicSize<pos_t>  Size;
    typedef Gfx::BasicRect<pos_t>  Rect;

    public:
        enum Rotation
        {
            Rotate0 = 0, //0 - normal orientation (0 degree)
            Rotate90,    //1 - clockwise orientation (90 degrees)
        };

    public:
        FrameBuffer();

        virtual ~FrameBuffer();

        const Gfx::ImageFormat& format() const 
        {
            return *_format;
        }

        Rotation rotation() const
        {
          return _rotation;
        }

        void setRotation(Rotation r);

        size_t width() const;

        size_t height() const;

        Size size() const 
        {
            return Size( width(), height() );
        }

        size_t depth() const
        {
            return _screenInfo.bits_per_pixel;
        }

        size_t pixelSize() const
        {
            return _pixelSize;
        }

        size_t lineSize() const
        {
            return _lineSize;
        }

        size_t strideSize() const;

        void output( const Pt::uint8_t* frame, const Rect& area );

    private:
      inline char* pixelFB(size_t w, size_t h)
      {
          return &_buffer[(_fixedInfo.line_length * h) + _pixelSize * w];
      }

      inline const Pt::uint8_t* pixelFrame(const Pt::uint8_t* frame, size_t w, size_t h)
      {
          return &frame [_lineSize * h + w * _pixelSize];
      }

    private:
        int               _fd;
        fb_var_screeninfo _screenInfo;
        fb_fix_screeninfo _fixedInfo;

        Rotation          _rotation;

        size_t            _bufferSize;
        char*             _buffer;
        
        Gfx::ImageFormat* _format;

        size_t            _lineSize;
        size_t            _pixelSize;
};

} // namespace

} // namespace

#endif
