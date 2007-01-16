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
#ifndef PT_GFX_FILLCONVEXPOLYGON_H
#define PT_GFX_FILLCONVEXPOLYGON_H

#include "FillPolygon.h"
#include "ClipConvexPolygon.h"
#include "RasterBuffer.h"


namespace Pt {

namespace Gfx {

/** @brief Fill polygons on images

    This class implements FillPolygon and is specialised for the filling
    of convex polygons.
 */
class FillConvexPolygon : public FillPolygon
{
    public:
        void clip( const Math::Rect& viewPort, std::vector<Math::Point>& points );

		/** @brief Rasterizes a filles polygon

			A polygon described by a vector of points is reasterized to
			the given RasterBuffer. No clipping is performed
		*/
        void rasterize( std::vector<Math::Point> points, RasterBuffer& rasterBuffer );

		/** @brief Draw buffer content on images
		*/
        void output( ARgbImage& image, const Brush& brush,
                     const RasterBuffer& rasterBuffer );

		/** @brief Fill a polygon on an image

		    @see FillPolygon::draw
		*/
        virtual void draw( ARgbImage& image, const Brush& brush,
                           const std::vector<Math::Point>& points );

    private:
         ClipConvexPolygon       _clip;
         RasterBuffer            _rasterBuffer;
         std::vector<ARgbColor>  _colorBuffer;
};

} //namespace Gfx

} //namespace Pt

#endif
