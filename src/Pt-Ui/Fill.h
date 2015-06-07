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
#ifndef PT_UI_FILL_H
#define PT_UI_FILL_H

#include <Pt/Ui/Point.h>
#include <Pt/Ui/Brush.h>
#include <Pt/Ui/Image.h>

namespace Pt {
namespace Ui {

/** @brief Fill Pixel Buffer

    This class is the base class for function object that fill pixels of an
    image. Derived classes implement special filling algorithms such as
    filling with textures or solid fills.

 */
class PT_UI_API Fill
{
    public:
        /** @brief Destructor
        */
        virtual ~Fill()
        {}

        /** @brief Fill Pixels on an Image

            The pixels at the given X and y position are filled using the
            given brush. The origin of the geometric figure is also given
            for the algorithms that need a relative point.

            @param image The target image
            @param brush Brush for filling
            @param origin Origin of the whole figure
            @param xpos X cooridnate of the pixels to fill
            @param ypos Y coorinate of the pixels tro fill
            @param length number of pixels to fill
        */
        virtual void fill( Image& image, const Brush& brush,
                            const PointF& origin,
                            ssize_t xpos, ssize_t ypos, size_t length ) = 0;


};


/** @brief Texture Filling

    This class implements Fill and is specialised for filling pixels with
    the texture of a Brush.

 */
class PT_UI_API FillTexture : public Fill
{
    public:
        /** @sa Fill::fill

            The Texture of the brush is drawn relative to the origin passed
            to this function.
        */
        virtual void fill( Image& image, const Brush& brush,
                           const PointF& origin,
                           ssize_t xpos, ssize_t ypos, size_t length );
};


/** @brief Solid Filling

    This class implements Fill and is specialised for filling pixels with
    the color of a Brush.

 */
class PT_UI_API FillSolid : public Fill
{
    public:
        /** @sa Fill::fill
        */
        virtual void fill( Image& image, const Brush& brush,
                           const PointF& origin,
                           ssize_t xpos, ssize_t ypos, size_t length );
};

}} // namespace Pt

#endif
