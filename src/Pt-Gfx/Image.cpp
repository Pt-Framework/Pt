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

///////////////////////////////////////////////////////////////////////
// Image
///////////////////////////////////////////////////////////////////////

Image::Image()
: BasicImage( ImageFormat::argb32() )
{
}


Image::Image(const Image& image)
: BasicImage( image)
{
}


Image::Image(const ImageFormat& format)
: BasicImage(format)
{
}

Image::Image(const ImageFormat& format, 
             Pt::ssize_t width, Pt::ssize_t height, size_t padding)
: BasicImage(width, height, padding, format)
{
}


Image::Image(const ImageFormat& format, Pt::uint8_t* data,
             Pt::ssize_t width, Pt::ssize_t height, size_t padding)
: BasicImage(data, width, height, padding, format)
{
}


Image::~Image()
{
}


const Image& Image::operator=(const Image& image)
{
    BasicImage::operator=(image);
    return *this;
}

///////////////////////////////////////////////////////////////////////
// ConstImage
///////////////////////////////////////////////////////////////////////

//ConstImage::ConstImage()
//: BasicConstImage( ImageFormat::argb32() )
//{
//}
//
//
//ConstImage::ConstImage(const ConstImage& image)
//: BasicConstImage(image)
//{
//}
//
//
//ConstImage::ConstImage(const Image& image)
//: BasicConstImage( image )
//{
//}
//
//
//ConstImage::ConstImage(const ImageFormat& format)
//: BasicConstImage(format)
//{
//}
//
//
//ConstImage::ConstImage(const ImageFormat& format, const Pt::uint8_t* data,
//                       Pt::ssize_t width, Pt::ssize_t height, size_t padding)
//: BasicConstImage(format, data, width, height, padding)
//{
//}
//
//
//ConstImage::~ConstImage()
//{
//}

} // namespace

} // namespace
