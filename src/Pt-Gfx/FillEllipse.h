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
#ifndef PT_GFX_FILLELLIPSE_H
#define PT_GFX_FILLELLIPSE_H

#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Brush.h>


namespace Pt{

namespace Gfx{

class Fill;

/** @brief Fill ellipses on images

    This class is a function object that can fill ellipes.
    It delegates pixel filling to a Fill object.
 */
class FillEllipse
{
    public:
        /** @brief Default constructor
        */
        FillEllipse();

        /** @brief Destructor
        */
        ~FillEllipse();

        /** @brief Set Fill routine

            The set Fill object is used to perform the actuall filling
            of the spans and pixels.

            @param f Fill object to use
        */
        void setOutput(Fill& f)
        { _fill = & f; }

        /** @brief Fill an ellipse

            The ellipse described by its top-left point and size will be
            filled on an ARgbImage. The attributes for the fill operation
            are taken from the passed Brush object.

            @param image Target image
            @param pen Brush to be used
            @param topLeft Top-left point of the ellipse
            @param size Size of the ellipse
        */
        void draw( ARgbImage& image, const Brush& brush, const Pt::Math::Point& topLeft, const Pt::Math::Size& size );

    private:
        Fill* _fill;
        void outputSpan( ARgbImage& image, const Brush& brush, const Pt::Math::Point& topLeft, int x, int y, int width );
};

}

}

#endif
