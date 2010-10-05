/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *   Copyright (C) 2010 Aloysius Indrayanto                                *
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
#ifndef PT_GFX_FILL_H
#define PT_GFX_FILL_H

#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Gfx.h>

namespace Pt {

namespace Gfx {

/** @brief Fill Pixel Buffer

    This class is the base class for function object that fill pixels of an
    image. Derived classes implement special filling algorithms such as
    filling with textures or solid fills.

 */
class PT_GFX_API Fill
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
        virtual void fill( Pt::Gfx::ARgbImage& image, const Brush& brush,
                            const Gfx::Point& origin,
                            ssize_t xpos, ssize_t ypos, size_t length ) = 0;


};


/** @brief Texture Filling

    This class implements Fill and is specialised for filling pixels with
    the texture of a Brush.

 */
class PT_GFX_API FillTexture : public Fill
{
    public:
        /** @sa Fill::fill

            The Texture of the brush is drawn relative to the origin passed
            to this function.
        */
        virtual void fill( Pt::Gfx::ARgbImage& image, const Brush& brush,
                           const Gfx::Point& origin,
                           ssize_t xpos, ssize_t ypos, size_t length );
};


/** @brief Solid Filling

    This class implements Fill and is specialised for filling pixels with
    the color of a Brush.

 */
class PT_GFX_API FillSolid : public Fill
{
    public:
        /** @sa Fill::fill
        */
        virtual void fill( Pt::Gfx::ARgbImage& image, const Brush& brush,
                           const Gfx::Point& origin,
                           ssize_t xpos, ssize_t ypos, size_t length );
};

} // namespace Gfx

} // namespace Pt

#endif
