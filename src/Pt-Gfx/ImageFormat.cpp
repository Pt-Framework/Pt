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

#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/ImageInfo.h>
#include <Pt/Gfx/Argb8888Format.h>
#include <Pt/Gfx/Rgb888Format.h>
#include <Pt/Gfx/Rgb565Format.h>

namespace Pt {

namespace Gfx {

ImageFormat::ImageFormat( size_t pixelSize, size_t channels, bool planar)
: _pixelSize(pixelSize)
, _channels(channels)
, _planar(planar)
{
}


ImageFormat::~ImageFormat()
{
}

void ImageFormat::assign(Pixel& to, const Pixel& from) const
{
}

void ImageFormat::setPixel(Pixel& to, const Pixel& from,
                           CompositionMode mode) const
{
}


Color ImageFormat::getColor(const Pixel& pixel) const
{
    return Color(0,0,0);
}


void ImageFormat::setPixel(Pixel& pixel, const Color& c,
                           CompositionMode mode) const
{
}


void ImageFormat::setSpan(Pixel& dst, const Pixel& src, size_t length, CompositionMode mode) const
{

}

void ImageFormat::copy(const ImageInfo& toInfo, const Point& to,
                       const ImageInfo& fromInfo, const Rect& fromRect,
                       CompositionMode mode) const
{
    Rect clipRect(Point(0,0), toInfo.size());

    // clip fromRect to fit into the clip/image rect
    Point d = clipRect.topLeft() - to;
    Point fromPos = fromRect.topLeft() + d;

    Rect fromClip( fromPos, clipRect.size() );
    fromClip = fromRect.intersect(fromClip);

    // account for smaller fromRect
    Point toClip = to + (fromClip.topLeft() - fromRect.topLeft());
    
    onCopy(toInfo, toClip, fromInfo, fromClip, mode);
}


const ImageFormat& ImageFormat::rgb565()
{
	static const Rgb565Format f;
	return f;
}


const ImageFormat& ImageFormat::rgb888()
{
	static const Rgb888Format f;
	return f;

}


const ImageFormat& ImageFormat::argb8888()
{
	static const Argb8888Format f;
	return f;
}

} // namespace

} // namespace
