/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
