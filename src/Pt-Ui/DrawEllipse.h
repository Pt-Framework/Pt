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
#ifndef PT_UI_DRAWELLIPSE_H
#define PT_UI_DRAWELLIPSE_H

#include <Pt/Ui/Image.h>
#include <Pt/Ui/Pen.h>
#include <Pt/Ui/Point.h>
#include <Pt/Ui/Size.h>

namespace Pt{
namespace Ui{

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
        virtual void draw( Image& image, const Pen& pen, const PointF& topLeft, const SizeF& size) = 0;
};

} }//namespace

#endif
