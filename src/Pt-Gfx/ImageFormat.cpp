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
#include <Pt/Gfx/ImageView.h>
#include <Pt/Gfx/Argb32Format.h>
#include <Pt/Gfx/Rgb32Format.h>
#include <Pt/Gfx/Rgb16Format.h>
#include <cassert>

namespace Pt {

namespace Gfx {

void ImageFormat::copy(ImageView& to, Pt::ssize_t toX, Pt::ssize_t toY,
                       const ImageView& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                       Pt::ssize_t width, Pt::ssize_t height, 
                       CompositionMode mode) const
{
    bool outside = toX < 0 || toY < 0 ||
                   toX + width > to.width() ||
                   toY + height > to.height();

    assert( ! outside );

    //if(outside)
    //    return;

    //Rect clipRect(Point(0,0), to.size());

    //// clip fromRect to fit into the clip/image rect
    //Point d = clipRect.topLeft() - toPos;
    //Point fromPos = fromRect.topLeft() + d;

    //Rect fromClip( fromPos, clipRect.size() );
    //fromClip = fromRect.intersect(fromClip);

    //// account for smaller fromRect
    //Point toClip = toPos + (fromClip.topLeft() - fromRect.topLeft());
    
    //onCopy(to, toClip, from, fromClip, mode);

    onCopy(to, toX, toY, from, fromX, fromY, width, height, mode);
}


const ImageFormat& ImageFormat::rgb16()
{
	static const Rgb16Format f;
	return f;
}


const ImageFormat& ImageFormat::rgb32()
{
	static const Rgb32Format f;
	return f;
}


const ImageFormat& ImageFormat::argb32()
{
	static const Argb32Format f;
	return f;
}

} // namespace

} // namespace
