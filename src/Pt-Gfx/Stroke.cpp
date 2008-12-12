/*
 * Copyright (C) 2006-2008 Laurentiu-Gheorghe Crisan
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
#include "Pt/Gfx/ARgbImage.h"
#include "Pt/Gfx/Pen.h"
#include "Stroke.h"

namespace Pt{
namespace Gfx{

Stroke::Stroke()
{ }

Stroke::~Stroke()
{ }
        
void Stroke::stroke( Pt::Gfx::ARgbImage& image, const Pen& pen, ssize_t xpos, ssize_t ypos)
{
    if( xpos < 0  || xpos >= image.width())
        return;

    if( ypos <0 || ypos >= image.height())
        return;

    image.pixel(xpos,ypos) = pen.color();
}

void Stroke::stroke( Pt::Gfx::ARgbImage& image, const Pen& pen, ssize_t xpos, ssize_t ypos, size_t length )
{
    const Pt::Gfx::ARgbImage& colorBuffer = pen.buffer();

    //Clip the span
    if( ypos< 0 )
        return;

    if( ypos >= (Pt::ssize_t)image.height() )
        return;

    if( xpos >= (Pt::ssize_t) image.width() )
        return;

    if( xpos < 0 )
    {
        if(  static_cast<ssize_t>(length) > -xpos  )
        {
            length += xpos;
            xpos = 0;
        }
        else
        {
            return;
        }
    }

    if( (xpos + length) > image.width() )
        length =  image.width() - xpos;

    // Copy pixels blockwise to the target image.
    while(length)
    {
        const size_t fillLength = std::min( length, colorBuffer.width() );

        if(fillLength)
        {
            std::memcpy( &image.pixel( xpos, ypos ), colorBuffer.data(),
                         fillLength * sizeof(ARgbColor) );
        }

        length -= fillLength;
        xpos   += fillLength;
    }
}

}//namespace gfx
}//namespace Pt
