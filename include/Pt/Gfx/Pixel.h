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
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Color.h>

namespace Pt {
namespace Gfx {


class Pixel
{
    public:
        Pixel(const ImageInfo& info, int x, int y)
        { 
          reset( info, x, y);
        }

        Pixel(const Pixel& p)
        { 
          reset(p);
        }

        Pixel& operator=(const Pixel& p)
        {
            _info->format().assign(*this, p);
            return *this;
        }

        void advance()
        {
            if( ++_x >= _info->width() )
            {
                _x = 0;
                ++_y;
            }
        }

        void advance( int n )
        {
            Pt::ssize_t off = _x + n;
            _y += off / _info->width();
            _x += off % _info->width();
        }

        void reset(const ImageInfo& info, int x, int y)
        {
            _info = &info;
             _x = x;
             _y = y;
        }

        void reset(const Pixel& p)
        {
             _info = p._info;
             _x = p._x;
             _y = p._y;
        }

        const ImageInfo& imageInfo() const
        { 
          return *_info; 
        }
        
        int x() const
        {
            return _x;
        }

        int y() const 
        {
            return _y;
        }

    private:
        const ImageInfo* _info;
        int _x;
        int _y;
};

}}

#endif