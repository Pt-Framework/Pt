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

namespace Pt {

namespace Gfx {

Image::Image()
: BasicImage()
{
    BasicImage::reset( ImageFormat::argb32(), 0, 0, 0);
}


Image::Image(const ImageFormat& format)
: BasicImage()
{
    BasicImage::reset(format, 0, 0, 0);
}


Image::Image(const ImageFormat& format, 
             Pt::ssize_t width, Pt::ssize_t height, size_t padding)
: BasicImage()
{
    BasicImage::reset(format, width, height, padding);
}


Image::Image(const ImageFormat& format, Pt::uint8_t* data,
             Pt::ssize_t width, Pt::ssize_t height, size_t padding)
: BasicImage()
{
    BasicImage::reset(format, data, width, height, padding);
}


Image::Image(const Image& image)
: BasicImage()
{
    *this = image;
}


Image::~Image()
{
}


void Image::reset(const ImageFormat& format, Pt::ssize_t width, Pt::ssize_t height, 
                   Pt::ssize_t padding)
{
    BasicImage::reset(format, width, height, padding);
}


void Image::reset(const ImageFormat& format, Pt::uint8_t* data, 
                  Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding)
{
    BasicImage::reset(format, data, width, height, padding);
}


const Image& Image::operator=(const Image& image)
{
    const ImageFormat& format = image.format();

    BasicImage::reset( format, image.width(), image.height() , image.padding() );
    
    Pt::ssize_t n = format.imageSize( image.width(), image.height(), image.padding() );  
    if(n != 0)
    {
      const Pt::uint8_t* imageData = image.data();
      std::memcpy(data(), imageData, n);
    }

    return *this;
}

} // namespace

} // namespace
