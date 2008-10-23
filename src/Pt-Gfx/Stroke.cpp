/***************************************************************************
 *   Copyright (C) 2006-2008 Laurentiu-Gheorghe Crisan                     *
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
