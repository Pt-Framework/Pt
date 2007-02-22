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
#ifndef Pt_Gfx_Color_h
#define Pt_Gfx_Color_h

#include <limits>
#include <assert.h>

#include <Pt/Gfx/Api.h>
#include <Pt/IfElse.h>
#include <Pt/Types.h>


namespace Pt {

    namespace Gfx {

        /** @brief An empty structure used for tagging the type of the color class.
         */
        struct ContainerType {};

        /** @brief An empty structure used for tagging the type of the color class.
         */
        struct ReferenceType {};


        /** @brief Basic template declaration of color classes.
         *  @ingroup Gfx
         */
        template <typename TagT, typename TypeT = ContainerType>
        class Color;

        /** @brief Basic template declaration of color traits classes.
         */
        template <typename ColorT>
        struct ColorTraits;


        /** @brief Choose the type which has greater size (from the two given types).
         */
        template <typename A, typename B>
        struct LargestSizeOf {
            typedef typename IfElse< (sizeof(A) >= sizeof(B)), A, B >::ResultT Result;
        };


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

