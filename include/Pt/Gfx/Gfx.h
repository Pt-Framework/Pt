/***************************************************************************
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
 *   Copyright (C) 2006-2007 by Marc Boris Dürner                          *
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
#ifndef Pt_Gfx_Gfx_h
#define Pt_Gfx_Gfx_h

#include <Pt/Types.h>
#include <Pt/Gfx/Api.h>


namespace Pt {

    namespace Gfx {

        //
        // Foward declarations of drawing classes
        //

        class Pen;
        class Brush;
        class Font;
        class FontMetrics;
        class Region;


        //
        // Foward declarations of color classes
        //

        struct ARgb;
        struct ARgbF;
        struct ARgb8888;
        struct Rgb888;
        struct Rgb565;
        struct Rgb555;

        template <typename TagT>
        class Color;

        template <typename ColorT>
        struct ColorTraits;

        typedef Color<ARgb>     ARgbColor;
        typedef Color<ARgbF>    ARgbFColor;
        typedef Color<ARgb8888> ARgb8888Color;
        typedef Color<Rgb888>   Rgb888Color;
        typedef Color<Rgb565>   Rgb565Color;
        typedef Color<Rgb555>   Rgb555Color;


        //
        // Foward declarations of standards interleaved image classes
        //

        template <typename ColorT_, typename ColorTraitsT_>
        class InterleavedImage;

        typedef InterleavedImage<ARgbColor,     ColorTraits<ARgbColor> >     ARgbInterleavedImage;
        typedef InterleavedImage<ARgbFColor,    ColorTraits<ARgbFColor> >    ARgbFInterleavedImage;
        typedef InterleavedImage<ARgb8888Color, ColorTraits<ARgb8888Color> > ARgb8888InterleavedImage;
        typedef InterleavedImage<Rgb888Color,   ColorTraits<Rgb888Color> >   Rgb888InterleavedImage;
        typedef InterleavedImage<Rgb565Color,   ColorTraits<Rgb565Color> >   Rgb565InterleavedImage;
        typedef InterleavedImage<Rgb555Color,   ColorTraits<Rgb555Color> >   Rgb555InterleavedImage;


        //
        // Foward declarations of standards interleaved subimage classes
        //

        template <typename ImageT_>
        class SubImage;

        typedef SubImage<ARgbInterleavedImage>     ARgbInterleavedSubImage;
        typedef SubImage<ARgbFInterleavedImage>    ARgbFInterleavedSubImage;
        typedef SubImage<ARgb8888InterleavedImage> ARgb8888InterleavedSubImage;
        typedef SubImage<Rgb888InterleavedImage>   Rgb888InterleavedSubImage;
        typedef SubImage<Rgb565InterleavedImage>   Rgb565InterleavedSubImage;
        typedef SubImage<Rgb555InterleavedImage>   Rgb555InterleavedSubImage;


        //
        // Interleaved images and subimages will be the most used types
        // and so typedefs them for convenience
        //

        typedef ARgbInterleavedImage     ARgbImage;
        typedef ARgbFInterleavedImage    ARgbFImage;
        typedef ARgb8888InterleavedImage ARgb8888Image;
        typedef Rgb888InterleavedImage   Rgb888Image;
        typedef Rgb565InterleavedImage   Rgb565Image;
        typedef Rgb555InterleavedImage   Rgb555Image;

        typedef ARgbInterleavedSubImage     ARgbSubImage;
        typedef ARgbFInterleavedSubImage    ARgbFSubImage;
        typedef ARgb8888InterleavedSubImage ARgb8888SubImage;
        typedef Rgb888InterleavedSubImage   Rgb888SubImage;
        typedef Rgb565InterleavedSubImage   Rgb565SubImage;
        typedef Rgb555InterleavedSubImage   Rgb555SubImage;

    } // namespace Gfx

} // namespace Pt

#endif
