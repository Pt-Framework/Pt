/*
 * Copyright (C) 2006-2010 by Aloysius Indrayanto
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
#ifndef Pt_Gfx_ImageAlgo_h
#define Pt_Gfx_ImageAlgo_h

#include <Pt/Gfx/ColorAlgo.h>
#include <Pt/Gfx/Size.h>


namespace Pt {

    namespace Gfx {

        /** @brief Assign one iterator range to another.
        */
        template <typename InputIteratorT, typename OutputIteratorT> inline
        OutputIteratorT assign(InputIteratorT begin, InputIteratorT end, OutputIteratorT to)
        {
            for(; begin != end; ++to, ++begin) assign(*to, *begin);
            return to;
        }


        /** @brief Assign an image to another image with a different color model.
        *
        *  An image classes implementor should specialize this function as needed if
        *  faster implementation for the two classes is exist.
        */
        template <typename ColorA, typename ColorB>
        void assign(InterleavedImage<ColorA>& to, const InterleavedImage<ColorB>& from);


        /** @brief Partial specialization of assign() if both the color models are the same.
        *
        *  This function will just copy the value from the source to the destiantion.
        */
        template <typename ColorT> inline
        void assign(InterleavedImage<ColorT>& to, const InterleavedImage<ColorT>& from)
        { to = from; }


        /** @brief Greyscale a pixel range using iterators.
        */
        template <typename InIteratorT, typename OutIteratorT> inline
        void greyscale(InIteratorT begin, InIteratorT end, OutIteratorT to)
        { for(; begin != end; ++begin, ++to) greyscale(*begin, *to); }


        /** @brief Greyscales an image using its iterators.
        */
        template<typename IteratorT> inline
        void greyscale(IteratorT begin, IteratorT end)
        { for(; begin != end; ++begin) greyscale(*begin); }


        /** @brief Greyscales an image using its iterators.
        */
        template<typename ImageT> inline
        void greyscale(ImageT& image)
        { greyscale(image.begin(), image.end); }


        /** @brief Block-scale a pixel range.
        *
        *  @param from       Begin of the source range
        *  @param fromWidth  Width of the source range
        *  @param fromHeight Height of the source range
        *  @param to         Begin of the destination range
        *  @param fromWidth  Width of the destination range
        *  @param fromHeight Height of the destination range
        *
        *  This algorithm block-scales the source range to the destination range
        *  (both ranges are specified using an input iterator, width and height).
        */
        template<typename InIteratorT, typename OutIteratorT>
        void blockScale(InIteratorT  from, uint fromWidth, uint fromHeight,
                        OutIteratorT to,   uint toWidth,   uint toHeight);

        /** @brief Block-scale a pixel range.
        *
        *  @param from    Begin of the source range
        *  @param to      Begin of the destination range
        *  @param fromEnd End of the source range
        *  @param toEnd   End of the destination range
        *
        *  This algorithm block-scales the source range [from, fromEnd] to the
        *  destination range [to, toEnd].
        */
        template <typename InIteratorT, typename OutIteratorT> inline
        void blockScale(InIteratorT  from, InIteratorT  fromEnd,
                        OutIteratorT to,   OutIteratorT toEnd)
        {
            const Gfx::Size fromSize   = fromEnd - from;
            const uint       fromWidth  = fromSize.width();
            const uint       fromHeight = fromSize.height();

            const Gfx::Size toSize   = toEnd - to;
            const uint       toWidth  = toSize.width();
            const uint       toHeight = toSize.height();

            blockScale(from, fromWidth, fromHeight, to, toWidth, toHeight);
        }

    #if 0
        /** @brief Block-scale an image.
        *
        *  @param from      Source image
        *  @param to        Destination image
        *  @param newWidth  Wanted width of the destination image
        *  @param newHeight Wanted height of the destination image
        */
        template <typename DstColorT, typename SrcColorT, typename DstColorTraitsT, typename SrcColorTraitsT> inline
        void blockScale(const InterleavedImage<SrcColorT, SrcColorTraitsT>& from, InterleavedImage<DstColorT, DstColorTraitsT>& to,
                        uint newWidth, uint newHeight)
        {
            to.resize(newWidth, newHeight);
            blockScale(from.begin(), from.width(), from.height(), to.begin(), newWidth, newHeight);
        }
    #endif

    } // namespace Gfx

} // namespace Pt


//
// Include the template implementation header
//
#include "ImageAlgo.tpp"

#endif

