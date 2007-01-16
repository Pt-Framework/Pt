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

#include <vector>
#include "DrawPolyline.h"
#include "DrawThinLine.h"


namespace Pt {

namespace Gfx {

/** @brief Draw thin polylines on an image

    This class implements DrawPolyline and is specialised for the drawing
    of thin polylines.
 */
class DrawThinPolyline : public DrawPolyline
{
    public:
        /** @brief Default constructor
        */
        DrawThinPolyline();

		/** @brief Draw a polyline on an image

			@see DrawPolyline::draw
		*/
        virtual void draw( ARgbImage& image,  const Pen& pen,
                           const std::vector<Math::Point>& points );

    private:
        DrawThinLine _drawThinLine;
};

} //namespace gfx

} //namespace Pt

#endif
