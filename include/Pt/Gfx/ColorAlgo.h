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
#ifndef Pt_Gfx_ColorAlgo_h
#define Pt_Gfx_ColorAlgo_h


namespace Pt {

    namespace Gfx {

        //
        // Foward declaration of some color-related classes
        //
        struct ARgb;
        template <typename TagT> class Color;


        /** @brief Assign a color model to another color model.
         *
         *  A color models implementor should specialize this function as needed if
         *  the faster implementation for the two colors is exist.
         */
        template <typename DstTagT, typename SrcTagT> inline
        void assign(Color<DstTagT>& to, const Color<SrcTagT>& from)
        {
            Color<ARgb> tmp;
            fromARgb(to, toARgb(tmp, from) );
        }

        /** @brief Partial specialization of assign() if both the color models are the same.
         *
         *  This function will just copy the value from the source to the destiantion.
         */
        template <typename TagT> inline
        void assign(Color<TagT>& to, const Color<TagT>& from)
        { to = from; }


        /** @brief Make the greyscale version of the source color.
         *
         *  This is a fallback version in case the color model implementor does not
         *  implement the specific version of greyscale() for the color model.
         *  \n\n
         *  A color implementor must not rely on this function since this function
         *  will cause some overhead because of the conversion to and from ARgbColor.
         */
        template <typename TagT> inline
        Color<TagT>& greyscale(Color<TagT>& to, const Color<TagT>& from)
        {
            Color<ARgb> tmp;

            toARgb(tmp, from);
            greyscale(tmp, tmp);
            fromARgb(to, tmp);

            return to;
        }

        /** @brief Make the given color become greyscale.
         */
        template <typename TagT> inline
        Color<TagT>& greyscale(Color<TagT>& c)
        { return greyscale(c, c); }

    } // namespace Gfx

} // namespace Pt

#endif

