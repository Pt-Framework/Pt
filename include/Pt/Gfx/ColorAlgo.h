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
#ifndef Pt_Gfx_ColorAlgo_h
#define Pt_Gfx_ColorAlgo_h


namespace Pt {

    namespace Gfx {

        //
        // Foward declaration of some color-related classes
        //
        struct ContainerType;
        struct ARgb;
        template <typename TagT, typename TypeT> class Color;


        /** @brief Assign a color model to another color model.
         *
         *  A color models implementor should specialize this function as needed if
         *  the faster implementation for the two colors is exist.
         */
        template <typename DstTagT, typename DstTypeT, typename SrcTagT, typename SrcTypeT> inline
        void assign(Color<DstTagT, DstTypeT>& to, const Color<SrcTagT, SrcTypeT>& from)
        {
            Color<ARgb, ContainerType> tmp;
            fromARgb(to, toARgb(tmp, from) );
        }

        /** @brief Partial specialization of assign() if both the color models are the same.
         *
         *  This function will just copy the value from the source to the destiantion.
         */
        template <typename TagT, typename TypeT> inline
        void assign(Color<TagT, TypeT>& to, const Color<TagT>, TypeT& from)
        { to = from; }


        /** @brief Make the greyscale version of the source color.
         *
         *  This is a fallback version in case the color model implementor does not
         *  implement the specific version of greyscale() for the color model.
         *  \n\n
         *  A color implementor must not rely on this function since this function
         *  will cause some overhead because of the conversion to and from ARgbColor.
         */
        template <typename TagT, typename TypeT> inline
        Color<TagT, TypeT>& greyscale(Color<TagT, TypeT>& to, const Color<TagT, TypeT>& from)
        {
            Color<ARgb, ContainerType> tmp;

            toARgb(tmp, from);
            greyscale(tmp, tmp);
            fromARgb(to, tmp);

            return to;
        }

        /** @brief Make the given color become greyscale.
         */
        template <typename TagT, typename TypeT> inline
        Color<TagT, TypeT>& greyscale(Color<TagT, TypeT>& c)
        { return greyscale(c, c); }

    } // namespace Gfx

} // namespace Pt

#endif

