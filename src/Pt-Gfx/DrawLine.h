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
#ifndef PT_GFX_DRAWLINE_H
#define PT_GFX_DRAWLINE_H

#include <vector>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/ARgbInterleavedImage.h>
#include <Pt/Math/Point.h>


namespace Pt {

namespace Gfx {

/** @brief Draw lines on an image

    This class is an interface for all function objects that can
    draw lines.
 */
class DrawLine
{
    public:
    	/** @brief Default Constructor
    	*/
        DrawLine()
        : _colorBuffer( 0 )
        { }

    	/** @brief Destructor
    	*/
        virtual ~DrawLine()
        { }

		/** @brief Draw a line on an image

			@see DrawLine::draw
		*/
		void operator() ( ARgbImage& image, const Pen& pen,
                          const Math::Point& from,
                          const Math::Point& to )
		{ this->draw(image, pen, from, to); }

		/** @brief Draw a line on an image

		    The line described by a two points will be drawn on an ARgbImage.
		    The attributes for the line are taken from the passed Pen object.
			Clipping has to be performed before the line is drawn.

		    @param image Target image
		    @param pen Pen to be used
		    @param from Begin of the line
		    @param end End of the line
		*/
        virtual void draw( ARgbImage& image, const Pen& pen,
                           const Math::Point& from,
                           const Math::Point& to ) = 0;

    protected:
        std::vector<ARgbColor>  _colorBuffer;
};

} // namespace Gfx

} // namespace Pt

#endif
