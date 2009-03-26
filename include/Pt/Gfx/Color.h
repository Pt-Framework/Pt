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
#ifndef Pt_Gfx_Color_h
#define Pt_Gfx_Color_h

#include <Pt/Gfx/GenericAlgo.h>

namespace Pt {

    namespace Gfx {

        /** @brief Basic template declaration of color classes.
         *  @ingroup Gfx
         */
        template <typename TagT>
        class Color;

        /** @brief Basic template declaration of color traits classes.
         */
        template <typename ColorT>
        struct ColorTraits;


        /** @brief Greater-than operator for any color model comparison.
         *
         *  Note that by default, this function will use operator==() and operator<().
         *  \n\n
         *  A color model implementor should implement the full specialization of
         *  this function for better performance.
         */
        template <typename TagT> inline
        bool operator>(const Color<TagT>& c1, const Color<TagT>& c2)
        { return !(c1==c2) && !(c1<c2); }

        /** @brief Inequality operator for any color model comparison.
         *
         *  Note that by default, this function will call operator==().
         *  \n\n
         *  A color model implementor can implement the full specialization of
         *  this function if better performance can be achieved by doing so.
         */
        template <typename TagT> inline
        bool operator!=(const Color<TagT>& c1, const Color<TagT>& c2)
        { return !(c1==c2); }

        /** @brief Less-than-or-equal operator for any color model comparison.
         *
         *  Note that by default, this function will call operator==() and operator<().
         *  \n\n
         *  A color model implementor can implement the full specialization of
         *  this function if better performance can be achieved by doing so.
         */
        template <typename TagT> inline
        bool operator<=(const Color<TagT>& c1, const Color<TagT>& c2)
        { return (c1==c2) || (c1<c2); }

        /** @brief Greater-than-or-equal operator for color comparison.
         *
         *  Note that by default, this function will call operator<().
         *  \n\n
         *  A color model implementor can implement the full specialization of
         *  this function if better performance can be achieved by doing so.
         */
        template <typename TagT> inline
        bool operator>=(const Color<TagT>& c1, const Color<TagT>& c2)
        { return !(c1<c2); }


        /** @brief Addition operator for any color model mathematics (beware of overflow).
         *
         *  Note that by default, this function will call operator=() and operator+=().
         *  \n\n
         *  A color model implementor can implement the full specialization of
         *  this function if better performance can be achieved by doing so.
         */
        template <typename TagT> inline
        const Color<TagT>& operator+(const Color<TagT>& c1, const Color<TagT>& c2)
        {
            TagT rs = c1;
            rs += c2;
            return(rs);
        }

        /** @brief Addition operator for any color model mathematics (beware of underflow).
         *
         *  Note that by default, this function will call operator=() and operator-=().
         *  \n\n
         *  A color model implementor can implement the full specialization of
         *  this function if better performance can be achieved by doing so.
         */
        template <typename TagT> inline
        const Color<TagT>& operator-(const Color<TagT>& c1, const Color<TagT>& c2)
        {
            TagT rs = c1;
            rs -= c2;
            return(rs);
        }

    } // namespace Gfx

} // namespace Pt

#endif

