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

        struct ContainerType;
        struct ReferenceType;

        template <typename TagT, typename TypeT>
        class Color;

        template <typename ColorT>
        struct ColorTraits;


        struct ARgb;
        struct ARgbF;
        struct ARgb8888;
        struct Rgb888;
        struct Rgb565;
        struct Rgb555;

        typedef Color<ARgb,     ContainerType> ARgbColor;
        typedef Color<ARgbF,    ContainerType> ARgbFColor;
        typedef Color<ARgb8888, ContainerType> ARgb8888Color;
        typedef Color<Rgb888,   ContainerType> Rgb888Color;
        typedef Color<Rgb565,   ContainerType> Rgb565Color;
        typedef Color<Rgb555,   ContainerType> Rgb555Color;

        //typedef Color<ARgb,     ReferenceType> ARgbColorRef;
        typedef Color<ARgbF,    ReferenceType> ARgbFColorRef;
        typedef Color<ARgb8888, ReferenceType> ARgb8888ColorRef;
        typedef Color<Rgb888,   ReferenceType> Rgb888ColorRef;
        typedef Color<Rgb565,   ReferenceType> Rgb565ColorRef;
        typedef Color<Rgb555,   ReferenceType> Rgb555ColorRef;


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
