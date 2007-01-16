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
#ifndef PT_GFX_DRAWTHICKLINE_H
#define PT_GFX_DRAWTHICKLINE_H

#include <vector>
#include <Pt/Gfx/Brush.h>

#include "DrawLine.h"
#include "Span.h"
#include "FillConvexPolygon.h"
#include "RasterBuffer.h"


namespace Pt {

namespace Gfx {

/** @brief Draw thick lines on an image

    This class implements DrawLine and is specialised for the  drawing
    of thick lines.
 */
class DrawThickLine : public DrawLine
{
    public:
        /** @brief Default constructor
        */
        DrawThickLine();

		/** @brief Rasterizes a thick line

			A line described by two points is reasterized to the given
			RasterBuffer. Attributes for the line are taken from the pen.
			Clipping is performed before the line is drawn.
		*/
        void rasterize( ARgbImage& image, const Pen& pen, const Math::Point& from,
                        const Math::Point to, RasterBuffer& rasterBuffer );

		/** @brief Draw a line on an image

			@see DrawLine::draw
		*/
        void draw( ARgbImage& image, const Pen& pen, const Math::Point& from, const Math::Point& to );

    private:
        FillConvexPolygon _fillConvexPolygon;
        RasterBuffer      _rasterBuffer;
};

} // namespace Gfx

} // namespace Pt

#endif
