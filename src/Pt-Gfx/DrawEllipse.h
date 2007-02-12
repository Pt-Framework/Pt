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
#ifndef PT_DRAWELLIPSE_H
#define PT_DRAWELLIPSE_H

#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Pen.h>

namespace Pt{
namespace Gfx{

/** @brief Draw ellipses on images

    This class is an interface for all function objects that can
    draw ellipses. It delegates pixel filling to a Fill object.
 */
class DrawEllipse
{
    public:
        /** @brief Default constructor
        */
        DrawEllipse()
        {}

        /** @brief Destructor
        */
        virtual ~DrawEllipse()
        {}

        /** @brief Draw an ellipse

            The ellipse described by its top-left point and size will be
            drawn on an ARgbImage. The attributes for the fill operation
            are taken from the passed Pen object.

            @param image Target image
            @param pen Pen to be used
            @param topLeft Top-left point of the ellipse
            @param size Size of the ellipse
        */
        virtual void draw( ARgbImage& image, const Pen& pen, const Pt::Math::Point& topLeft, const Pt::Math::Size& size) = 0;
};

}//namespace Gfx
}//namepsace Pt

#endif
