/*
 * Copyright (C) 2006-2007 by Aloysius Indrayanto
 * Copyright (C) 2006-2007 by Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef Pt_Gfx_Gfx_h
#define Pt_Gfx_Gfx_h

#include <Pt/Types.h>
#include <Pt/Gfx/Api.h>

#include <memory>


namespace Pt {

    /** @namespace Pt::Gfx
        @brief Graphics and Imaging

        A 2D painting API is provided that can be implemented to
        draw on different output targets with an implementation to
        draw on off-screen images. Image classes support planar
        and interleaved images and algortihms to manipulate them.
    */
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
        template <typename TagT>
        class Color;

        struct ARgb;
        struct ARgbF;
        struct ARgb8888;
        struct Rgb888;
        struct Rgb565;
        struct Rgb555;

        typedef Color<ARgb    > ARgbColor;
        typedef Color<ARgbF   > ARgbFColor;
        typedef Color<ARgb8888> ARgb8888Color;
        typedef Color<Rgb888  > Rgb888Color;
        typedef Color<Rgb565  > Rgb565Color;
        typedef Color<Rgb555  > Rgb555Color;

        struct ARgbProxy;
        struct ARgbFProxy;
        struct ARgb8888Proxy;
        struct Rgb888Proxy;
        struct Rgb565Proxy;
        struct Rgb555Proxy;

        typedef Color<ARgbProxy    > ARgbColorProxy;
        typedef Color<ARgbFProxy   > ARgbFColorProxy;
        typedef Color<ARgb8888Proxy> ARgb8888ColorProxy;
        typedef Color<Rgb888Proxy  > Rgb888ColorProxy;
        typedef Color<Rgb565Proxy  > Rgb565ColorProxy;
        typedef Color<Rgb555Proxy  > Rgb555ColorProxy;


        //
        // Foward declarations of standards interleaved image and subimage classes
        //
        template <typename ColorT_>
        class InterleavedImage;

        typedef InterleavedImage<ARgbColor>     ARgbInterleavedImage;
        typedef InterleavedImage<ARgbFColor>    ARgbFInterleavedImage;
        typedef InterleavedImage<ARgb8888Color> ARgb8888InterleavedImage;
        typedef InterleavedImage<Rgb888Color>   Rgb888InterleavedImage;
        typedef InterleavedImage<Rgb565Color>   Rgb565InterleavedImage;
        typedef InterleavedImage<Rgb555Color>   Rgb555InterleavedImage;

        template <typename ImageT_>
        class InterleavedSubImage;

        typedef InterleavedSubImage<ARgbInterleavedImage>     ARgbInterleavedSubImage;
        typedef InterleavedSubImage<ARgbFInterleavedImage>    ARgbFInterleavedSubImage;
        typedef InterleavedSubImage<ARgb8888InterleavedImage> ARgb8888InterleavedSubImage;
        typedef InterleavedSubImage<Rgb888InterleavedImage>   Rgb888InterleavedSubImage;
        typedef InterleavedSubImage<Rgb565InterleavedImage>   Rgb565InterleavedSubImage;
        typedef InterleavedSubImage<Rgb555InterleavedImage>   Rgb555InterleavedSubImage;


        //
        // Foward declarations of standards planar image and subimage classes
        //
        template<typename ColorProxyT_, uint8_t SubSamplingX_, uint8_t SubSamplingY_>
        class PlanarImageModel;

        template <typename PlanarImageModelT_, typename AllocatorT = std::allocator<unsigned char> >
        class PlanarImage;

        typedef PlanarImage< PlanarImageModel<ARgbProxy, 1, 1> > ARgbPlanarImage;

        template <typename ImageT_>
        class PlanarSubImage;

        typedef PlanarSubImage<ARgbPlanarImage> ARgbPlanarSubImage;


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

        template <typename PointT, typename SizeT>
        class BasicRect;

        typedef BasicRect<Pt::ssize_t, Pt::size_t>  Rect;
        typedef BasicRect<double, double>           RectF;


    } // namespace Gfx

} // namespace Pt

#endif
