/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Fill.h"

#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Brush.h>

#include <algorithm>


namespace Pt {

namespace Gfx {

void FillTexture::fill( Pt::Gfx::ARgbImage& image, const Brush& brush,
                        const Math::Point& origin,
                        ssize_t xpos, ssize_t ypos, size_t length )
{
    const Pt::Gfx::ARgbImage& texture = brush.texture();

    while(length)
    {
        // x position in the texture to copy from
        const size_t textureXPos = ( xpos - origin.x() ) % texture.width();

        // determine the scanline of the texture to copy from
        const size_t textureYPos = ( ypos-origin.y() ) % texture.height();

        // number of pixels to copy from texture
        const size_t fillLength = std::min( length, texture.width() - textureXPos );

        // Copy pixels from textrure to image
        if(fillLength)
        {
            std::memcpy( &image.pixel( xpos, ypos ),
                         &texture.pixel(textureXPos, textureYPos),
                         fillLength * sizeof(ARgbColor) );
        }

        // Remaining unfilled pixels of the span
        length -= fillLength;
        xpos   += fillLength;
    }
}


void FillSolid::fill( Pt::Gfx::ARgbImage& image, const Brush& brush,
                      const Math::Point& origin,
                      ssize_t xpos, ssize_t ypos, size_t length )
{
    const Pt::Gfx::ARgbImage& texture = brush.texture();

    // copy pixels blockwise to the target image
    while(length)
    {
        const size_t fillLength = std::min( length, texture.width() );

        if(fillLength)
        {
            std::memcpy( &image.pixel( xpos, ypos ),
                         brush.texture().data(),
                         fillLength * sizeof(ARgbColor) );
        }

        length -= fillLength;
        xpos   += fillLength;
    }
}

} // namespace Gfx

} // namespace Pt
