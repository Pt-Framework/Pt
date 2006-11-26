/***************************************************************************
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
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
#ifndef Pt_Algorithm_h
#define Pt_Algorithm_h

#include <Pt/Gfx/BasicImage.h>

#include <iostream>


namespace Pt {

	namespace Gfx {

		//! @brief Transforms one sequence into another.
		template <typename In, typename Out, typename Op>
		inline Op transform(In begin, In end, Out dest, Op op)
		{
			for(; begin != end; ++begin, ++dest) op(*dest, *begin);
			return op;
		}


		//! @brief Transforms one sequence into another.
		template <typename Iter, typename Op>
		inline Op transform(Iter begin, Iter end, Op op)
		{
			for(; begin != end; ++begin) op(*begin);
			return op;
		}


		//! @brief assigns one iterator range to another
		template <typename InputIteratorT, typename OutputIteratorT>
		inline OutputIteratorT assign(InputIteratorT begin, InputIteratorT end, OutputIteratorT to)
		{
			for(; begin != end; ++to, ++begin) assign(*to, *begin);
			return to;
		}


		//! @brief Greyscales an image
		template<typename IteratorT>
		inline void greyscale(IteratorT begin, IteratorT end)
		{ for(; begin != end; ++begin) greyscale(*begin); }


		//! @brief Greyscale a pixel range.
		template <typename In, typename Out>
		inline void greyscale(In begin, In end, Out to)
		{ for(; begin != end; ++begin, ++to) greyscale(*begin, *to); }


		/** @brief Block-scale a pixel range.

		    @param from Begin of the source range
		    @param fromWidth Width of the source range
		    @param fromHeight Height of the source range
		    @param to Begin of the destination range
		    @param fromWidth Width of the destination range
		    @param fromHeight Height of the destination range

		    This algorithm block-scales the source range [from, fromEnd] to the
		    destination range [to, toEnd]
		*/
		template<typename In, typename Out>
		void blockScale(In from, size_t fromWdth, size_t fromHeight,
		                Out to,  size_t toWdth, size_t toHeight);


		/** @brief Block-scale a pixel range.

		    @param from Begin of the source range
		    @param fromEnd End of the source range
		    @param to Begin of the destination range
		    @param toEnd End of the destination range

		    This algorithm block-scales the source range [from, fromEnd] to the
		    destination range [to, toEnd]
		*/
		template<typename In, typename Out>
		void blockScale(In from, In fromEnd, Out to, Out toEnd);


		/** @brief Block-scale an image

		    @param srcImage  Source image
		    @param dstImage  Destination image
		    @param newWidth  Wanted width of the destination image
		    @param newHeight Wanted height of the destination image
		 */
		/*template<typename SrcColorSpaceT, typename DstColorSpaceT> inline
		void blockScale(const BasicImage<SrcColorSpaceT>& srcImage,
		                BasicImage<DstColorSpaceT>& dstImage,
		                size_t newWidth, size_t newHeight)
		{
			dstImage.resize(newWidth, newHeight);
			blockScale(srcImage.begin(), srcImage.width(), srcImage.height(),
			           dstImage.begin(), newWidth, newHeight);
		}*/

	} // namespace Gfx

} // namespace Pt

#endif


// Include the implementation header
#include <Pt/Gfx/Algorithm.tpp>
