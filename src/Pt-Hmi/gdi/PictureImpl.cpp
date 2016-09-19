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
  MA 02110-1301 USA
*/

#include "PictureImpl.h"

namespace Pt {

namespace Hmi {

PictureImpl::PictureImpl()
: _maskData(0)
, _mask(0)
, _bitmapData(0)
, _bitmap(0)
, _width(0)
, _height(0)
{
}


PictureImpl::~PictureImpl()
{
    clear();
}


void PictureImpl::clear()
{
    if(_mask)
    {
       DeleteObject(_mask);
       _mask = 0;
       _maskData.clear();
    }

    if(_bitmap)
    {
      DeleteObject(_bitmap);
      _bitmap = 0;
      _bitmapData.clear();
    }

    _width = 0;
    _height = 0;
}


void PictureImpl::set(const Gfx::Image& image)
{
    clear();
    
    _width = image.width();
    _height = image.height();

    for( std::size_t y = 0; y < image.height(); ++y )
    {
        for( std::size_t x = 0; x < image.width(); ++x )
        {
            Gfx::ConstPixel pixel(image.view(), x, y);
            Gfx::Color color = image.format().getColor(pixel);
            
            const Pt::uint8_t r = color.red() / 257;
            const Pt::uint8_t g = color.green() / 257;
            const Pt::uint8_t b = color.blue() / 257;
            const Pt::uint8_t a = color.alpha() / 257;

            _bitmapData.push_back( (Pt::uint8_t) (a * b/255) );
            _bitmapData.push_back( (Pt::uint8_t) (a * g/255) );
            _bitmapData.push_back( (Pt::uint8_t) (a * r/255) );
            _bitmapData.push_back( (Pt::uint8_t) (a ) );
        }
    }

    _bitmap = CreateBitmap(_width, _height, 1, 4*8, (VOID*)&_bitmapData[0]);
}


void PictureImpl::set(const Gfx::Image& image, Pt::uint16_t alphaThreshold)
{
    clear();
    
    _width = image.width();
    _height = image.height();

    for( size_t y = 0; y < image.height(); ++y )
    {
        for( size_t x = 0; x < image.width(); ++x )
        {
            Gfx::ConstPixel pixel(image.view(), x, y);
            Gfx::Color color = image.format().getColor(pixel);

            if( color.alpha() <= alphaThreshold )
            {
                _maskData.push_back(0xff);
                _maskData.push_back(0xff);
                _maskData.push_back(0xff);
                _maskData.push_back(0xff);

                _bitmapData.push_back(0);
                _bitmapData.push_back(0);
                _bitmapData.push_back(0);
                _bitmapData.push_back(0);
            }
            else
            {
                _maskData.push_back(0);
                _maskData.push_back(0);
                _maskData.push_back(0);
                _maskData.push_back(0);

                _bitmapData.push_back( (Pt::uint8_t) (color.blue() / 257) );
                _bitmapData.push_back( (Pt::uint8_t) (color.green()/ 257) );
                _bitmapData.push_back( (Pt::uint8_t) (color.red()/ 257) );
                _bitmapData.push_back( 0xff);
            }
        }
    }

    _mask = CreateBitmap(_width, _height, 1, 4*8, (VOID*)&_maskData[0]);
    _bitmap = CreateBitmap(_width, _height, 1, 4*8, (VOID*)&_bitmapData[0]);
}

} // namespace

} // namespace
