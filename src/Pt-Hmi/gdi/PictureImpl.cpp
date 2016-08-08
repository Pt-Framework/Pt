/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan
  
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
  MA  02110-1301  USA
*/

#include "PictureImpl.h"

namespace Pt {
namespace Hmi {

PictureImpl::PictureImpl()
: _andMask(0)
, _xorMask(0)
, _width(0)
, _height(0)
{
}


void PictureImpl::set(const Gfx::Image& image, Gfx::RenderFlags::Type flags )
{
    clear();
    _width = image.width();
    _height = image.height();

    for( size_t y = 0; y < image.height(); ++y )
    {
        for( size_t x = 0; x < image.width(); ++x )
        {
            const Gfx::Color& color = image.color( x, y );

            if( color.alpha() == 0 )
            {
                // transparent
                _andMask.push_back(0xff);
                _andMask.push_back(0xff);
                _andMask.push_back(0xff);
                _andMask.push_back(0xff);

                _xorMask.push_back(0);
                _xorMask.push_back(0);
                _xorMask.push_back(0);
                _xorMask.push_back(0);
            }
            else
            {
                _andMask.push_back(0);
                _andMask.push_back(0);
                _andMask.push_back(0);
                _andMask.push_back(0);

                _xorMask.push_back( (Pt::uint8_t) (color.blue() * 255.0) );
                _xorMask.push_back( (Pt::uint8_t) (color.green() * 255.0) );
                _xorMask.push_back( (Pt::uint8_t) (color.red() * 255.0) );
                _xorMask.push_back( 0xff);
            }
        }
    }

    _hAndMask = CreateBitmap(_width, _height, 1, 4*8, (VOID*)&_andMask[0]);
    _hXorMask = CreateBitmap(_width, _height, 1, 4*8, (VOID*)&_xorMask[0]);
}


void PictureImpl::clear()
{
    if( _andMask.size() != 0)
    {
       DeleteObject(_hAndMask);
       _andMask.clear();
    }

    if( _xorMask.size() != 0)
    {
      DeleteObject(_hXorMask);
      _xorMask.clear();
    }

  _width = 0;
  _height = 0;
}


PictureImpl::~PictureImpl()
{
  clear();
}

}} // namespace
