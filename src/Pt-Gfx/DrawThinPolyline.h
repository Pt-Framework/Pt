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
#ifndef PT_GFX_DRAWTHINPOLYLINE_H
#define PT_GFX_DRAWTHINPOLYLINE_H

#include "DrawPolyline.h"
#include "ClipLine.h"

namespace Pt {
namespace Gfx {

/** @brief Draw thin lines on an image

    This class implements DrawPolyline and is specialised for the drawing
    of thin lines.
 */
class DrawThinPolyline : public DrawPolyline
{
    public:

        DrawThinPolyline();
        ~DrawThinPolyline();
        /** @brief Draw a line on an image

            @see DrawLine::draw
        */
        void draw( ARgbImage& image, const Pen& pen, const  Math::Point* points, size_t pointCount );

    private:
        ClipLine _clipLine;
        void drawLine( ARgbImage& image, const Pen& pen, const Math::Point& from, const Math::Point& to );  
        void drawSolid( ARgbImage& image, const Pen& pen, const Math::Point& from, const Math::Point& to );
        void drawPattern( ARgbImage& image, const Pen& pen, const Math::Point& from, const Math::Point& to, const std::vector<bool>& pattern );

        inline void outputSpan(ARgbImage& image, const Pen& pen, size_t x, size_t y, size_t length )
        {
        //            memcpy( &image.pixel( x,  y ), &_colorBuffer[0], length * sizeof( ARgbColor)  );
            _stroke->stroke( image, pen, x, y, length );
        }

        inline void outputPixel( ARgbImage& image, const Pen& pen, size_t x, size_t y )
        {
            image.pixel( x,  y ) = pen.color();
        }

        std::vector<bool> _dashPattern;
};

} // namespace gfx

} // namespace Pt

#endif
