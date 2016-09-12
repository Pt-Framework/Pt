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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef PT_GFX_PIXEL_H
#define PT_GFX_PIXEL_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageInfo.h>

namespace Pt {

namespace Gfx {

class Pixel
{
    public:
        Pixel(const ImageInfo& info, Pt::ssize_t x, Pt::ssize_t y)
        : _info(&info)
        , _x(x)
        , _y(y)
        { 
            _base = info.data() + info.stride() * y + x * info.pixelStride();
        }

        Pixel(const Pixel& p)
        : _info(p._info)
        , _base(p._base)
        , _x(p._x)
        , _y(p._y)
        {  }

        Pixel& operator=(const Pixel& p)
        {
            _info->format().setPixel(*this, p, CompositionMode::SourceCopy);
            return *this;
        }
        
        void reset(const ImageInfo& info, Pt::ssize_t x, Pt::ssize_t y)
        {
            _info = &info;
             _x = x;
             _y = y;

             _base = info.data() + info.stride() * _y + _x * info.pixelStride();
        }

        void reset(const Pixel& p)
        {
             _info = p._info;
             _base = p._base;
             _x = p._x;
             _y = p._y;
        }
        
        void advance()
        {
            if( ++_x >= _info->width() )
            {
                _x = 0;
                ++_y;

                _base += _info->padding();
            }

            _base += _info->pixelStride();
        }

        void advance( Pt::ssize_t n )
        {
            Pt::ssize_t off = _x + n;
            _y += off / _info->width();
            _x += off % _info->width();

            _base = _info->data() + _info->stride() * _y + _x * _info->pixelStride();
        }

        const ImageInfo& imageInfo() const
        { 
          return *_info; 
        }
        
        Pt::ssize_t x() const
        {
            return _x;
        }

        Pt::ssize_t y() const 
        {
            return _y;
        }

        Pt::uint8_t* base() const
        {
            return _base;
        }

    private:
        const ImageInfo* _info;
        Pt::uint8_t* _base;
        Pt::ssize_t _x;
        Pt::ssize_t _y;
};

} // namespace

} // namespace

#endif