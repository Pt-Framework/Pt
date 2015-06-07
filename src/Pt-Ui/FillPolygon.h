/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
 * Copyright (C) 2010 Aloysius Indrayanto
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef PT_UI_FILLPOLYGON_H
#define PT_UI_FILLPOLYGON_H

#include <Pt/Ui/ImagePainter.h>
#include <Pt/Ui/Image.h>
#include <Pt/Ui/Brush.h>
#include <Pt/Ui/Point.h>
#include <Pt/Ui/ClipPolygon.h>
#include "Edge.h"
#include "EdgeTable.h"
#include "Fill.h"

namespace Pt{
namespace Ui{

/** @brief Fill polygons on images

    This class is an interface for all function objects that can
    fill polygons. It delegates pixel filling to a Fill object.
 */
class FillPolygon
{
    public:
        /** @brief Default constructor
        */
        FillPolygon();

        /** @brief Destructor
        */
        virtual ~FillPolygon()
        { }

        /** @brief Set Fill routine

            The set Fill object is used to perform the actuall filling
            of the spans and pixels.

            @param f Fill object to use
        */
        void setOutput(Fill& f)
        { _fill = &f; };

        /** @brief Fill a polygon on an image

            @see FillPolygon::draw
        */
        void operator() ( Image& image, const Brush& brush,
                          const PointF* points, size_t pointCount )
        { this->draw(image, brush, points, pointCount); }

        /** @brief Fill a polygon on an image

            The polygon described by a vector of points will be filled on an
            ARgbImage. The attributes for the fill operation are taken from
            the passed Brush object.

            @param image Target image
            @param pen Brush to be used
            @param points Polygon points
        */
        virtual void draw( Image& image, const Brush& brush,
                           const PointF* points, size_t pointCount );

    private:
        Fill*             _fill;
        ClipPolygon       _clipper;
        EdgeSet           _globalEdgeTable;
        ActiveEdgeTable   _activeEdgeTable;
        EdgeSet::iterator _currentPos;
};

} //namespace Gfx
} //namespace Pt

#endif
