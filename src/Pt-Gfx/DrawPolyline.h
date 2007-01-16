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
#ifndef PT_GFX_DRAWPOLYLINE_H
#define PT_GFX_DRAWPOLYLINE_H

#include <vector>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/ARgbImage.h>


namespace Pt {

namespace Gfx {

/** @brief Draw polylines on an image

    This class is an interface for all function objects that can
    draw lines.
 */
class DrawPolyline
{
	public:
    	/** @brief Default Constructor
    	*/
		DrawPolyline()
		{ }

    	/** @brief Destructor
    	*/
		virtual ~DrawPolyline()
		{ }

		/** @brief Draw a polyline on an image

			@see DrawPolyline::draw
		*/
		void operator() ( ARgbImage& image, const Pen& pen,
                          const std::vector<Math::Point>& points )
		{ this->draw(image, pen, points); }

		/** @brief Draw a polyline on an image

		    The polyline described by a vector of points will be drawn on an
		    ARgbImage. The attributes for the lines are taken from the passed
		    Pen object.

		    @param image Target image
		    @param pen Pen to be used
		    @param points Polyline points
		*/
		virtual void draw( ARgbImage& image, const Pen& pen,
						   const std::vector<Math::Point>& points ) = 0;
};

} //namespace Pt

} //namespace Gfx

#endif
