/***************************************************************************
 *   Copyright (C) 2006 by Aloysius Indrayanto                             *
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
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
#ifndef Pt_Gfx2_BasicColorAlgorithm_h
#define Pt_Gfx2_BasicColorAlgorithm_h

#include <Pt/Api.h>
#include <Pt/Types.h>


namespace Pt {

	namespace Gfx {

		/** @brief Convert a ColorT to ARgbColor's components.
		 *
		 *  Note that by default, this function is just declared and not defined.
		 *
		 *  A color model implementor must implement the full specialization of
		 *  this function.
		 */
		template <typename ColorT> inline
		void toARgb(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const ColorT& from);

		/** @brief Faster (but less precision) version of toARgb().
		 *
		 *  Note that the default implementation will just actually call toARgb().
		 *
		 *  A color model implementor can fully specialize this function as needed.
		 */
		template <typename ColorT> inline
		void toARgb_fast(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const ColorT& from)
		{ toARgb(a, r, g, b, from); }

		/** @brief Convert ARgbColor's components to a ColorT.
		 *
		 *  Note that by default, this function is just declared and not defined.
		 *
		 *  A color model implementor must implement the full specialization of
		 *  this function.
		 */
		template <typename ColorT> inline
		void fromARgb(ColorT& to, const uint16_t a, const uint16_t r, const uint16_t g, const uint16_t b);

		/** @brief Faster (but less precision) version of fromARgb().
		 *
		 *  Note that the default implementation will just actually call fromARgb().
		 *
		 *  A color model implementor can fully specialize this function as needed.
		 */
		template <typename ColorT> inline
		void fromARgb_fast(ColorT& to, const uint16_t a, const uint16_t r, const uint16_t g, const uint16_t b)
		{ fromARgb(to, a, r, g, b); }


		/** @brief Assign a color model to another color model.
		 *
		 *  A color model implementor should specialize this function as needed if
		 *  faster implementation for the two colors is exist.
		 */
		template <typename DstColorT, typename SrcColorT> inline
		void assign(DstColorT& to, const SrcColorT& from)
		{
			uint16_t a, r, g, b;
			toARgb(a, r, g, b, from);
			fromARgb(to, a, r, g, b);
		}

		/** @brief Faster (but less precision) version of assign().
		 *
		 *  A color model implementor should specialize this function as needed if
		 *  faster implementation for the two colors is exist.
		 */
		template <typename DstColorT, typename SrcColorT> inline
		void assign_fast(DstColorT& to, const SrcColorT& from)
		{
			uint16_t a, r, g, b;
			toARgb_fast(a, r, g, b, from);
			fromARgb_fast(to, a, r, g, b);
		}

		/** @brief Partial specialization of assign() if both the color models are the same.
		 *
		 *  This function will just copy the value from the source to the destiantion.
		 */
		template <typename ColorT> inline
		void assign(ColorT& to, const ColorT& from)
		{ to = from; }

		/** @brief Partial specialization of assign_fast() if both the color models are the same.
		 *
		 *  This function will just copy the value from the source to the destiantion.
		 */
		template <typename ColorT> inline
		void assign_fast(ColorT& to, const ColorT& from)
		{ to = from; }


		/** @brief Equality operator for color comparison.
		 *
		 *  Note that by default, this function is just declared and not defined.
		 *
		 *  A color model implementor must implement the full specialization of
		 *  this function.
		 */
		template <typename ColorT> inline
		bool operator==(const ColorT& c1, const ColorT& c2);

		/** @brief Less-than operator for color comparison.
		 *
		 *  Note that by default, this function is just declared and not defined.
		 *
		 *  A color model implementor must implement the full specialization of
		 *  this function.
		 */
		template <typename ColorT> inline
		bool operator<(const ColorT& c1, const ColorT& c2);

		/** @brief Greater-than operator for color comparison.
		 *
		 *  Note that by default, this function will use operator==() and operator<().
		 *
		 *  A color model implementor should implement the full specialization of
		 *  this function for better performance.
		 */
		template <typename ColorT> inline
		bool operator>(const ColorT& c1, const ColorT& c2)
		{ return !(c1==c2) && !(c1<c2); }

		/** @brief Inequality operator for color comparison.
		 *
		 *  Note that by default, this function will use operator==().
		 *
		 *  A color model implementor can implement the full specialization of
		 *  this function if better performance can be achieved by doing so.
		 */
		template <typename ColorT> inline
		bool operator!=(const ColorT& c1, const ColorT& c2)
		{ return !(c1==c2); }

		/** @brief Less-than-or-equal operator for color comparison.
		 *
		 *  Note that by default, this function will use operator==() and operator<().
		 *
		 *  A color model implementor can implement the full specialization of
		 *  this function if better performance can be achieved by doing so.
		 */
		template <typename ColorT> inline
		bool operator<=(const ColorT& c1, const ColorT& c2)
		{ return (c1==c2) || (c1<c2); }

		/** @brief Greater-than-or-equal operator for color comparison.
		 *
		 *  Note that by default, this function will use operator<().
		 *
		 *  A color model implementor can implement the full specialization of
		 *  this function if better performance can be achieved by doing so.
		 */
		template <typename ColorT> inline
		bool operator>=(const ColorT& c1, const ColorT& c2)
		{ return !(c1<c2); }


		/** @brief Addition operator for color mathematics (beware of overflow).
		 */
		template <typename ColorT> inline
		const ColorT& operator+(const ColorT& c1, const ColorT& c2)
		{
			const ColorT rs = c1 + c2;
			return(rs);
		}

		/** @brief Addition operator for color mathematics (beware of underflow).
		 */
		template <typename ColorT> inline
		const ColorT& operator-(const ColorT& c1, const ColorT& c2)
		{
			const ColorT rs = c1 - c2;
			return(rs);
		}

	} // namespace Gfx

} // namespace Pt

#endif

