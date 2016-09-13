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

#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Yuv12Image.h>

namespace Pt {

namespace Gfx {
 
Image::Image(const ImageFormat& format)		
: _view(format)
{
}


Image::Image(const ImageFormat& format, const Size& size, size_t padding)
: _buffer( format.imageSize(size, padding) )
, _view( format, 
         _buffer.empty() ? 0 : &_buffer[0], 
         size, 
         padding )
{
}


Image::Image(const ImageFormat& format, Pt::uint8_t* buffer, 
             const Gfx::Size& size, size_t padding)
: _view(format, buffer, size, padding)
{
}


Image::Image(const Image& image)
: _view( image.format() )
{
    *this = image;
}


Image::~Image()		
{
}


const Image& Image::operator=(const Image& image)
{
    // TODO: it may be better to always copy the pixels

    _buffer = image._buffer;

    Pt::uint8_t* data = _buffer.empty() ? image._view.data()
                                        : &_buffer[0];

    _view.set(image.format(), data, image.size(), image.view().padding());

	  return *this;
}


void Image::reset(const ImageFormat& f, const Gfx::Size& size, size_t padding)
{
    Pt::ssize_t n = f.imageSize(size, padding);
    
    _buffer.resize(n); 
    
    if( _buffer.empty() )
      _view.set(f, 0, size, padding); 
    else
      _view.set(f, &_buffer[0], size, padding); 
}


void Image::reset(const ImageFormat& format, Pt::uint8_t* buffer, 
                  const Gfx::Size& size, size_t padding)
{
    _view.set(format, buffer, size, padding);
    _buffer.clear();  
}

} // namespace

} // namespace
