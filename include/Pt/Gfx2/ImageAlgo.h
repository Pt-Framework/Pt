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
#ifndef Pt_Gfx2_ImageAlgo_h
#define Pt_Gfx2_ImageAlgo_h

#include <Pt/Gfx2/ColorAlgo.h>
#include <Pt/Gfx2/ARgbInterleavedImage.h>


namespace Pt {

	namespace Gfx {

		/** @brief Assign an image to another image with a different color model.
		 *
		 *  An image classes implementor should specialize this function as needed if
		 *  faster implementation for the two classes is exist.
		 */
		template <typename DstColorTagT, typename SrcColorTagT>
		void assign(InterleavedImage<DstColorTagT>& to, const InterleavedImage<SrcColorTagT>& from);

		/** @brief Partial specialization of assign() if both the color models are the same.
		 *
		 *  This function will just copy the value from the source to the destiantion.
		 */
		template <typename ColorTagT> inline
		void assign(InterleavedImage<ColorTagT>& to, const InterleavedImage<ColorTagT>& from)
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
		void blockScale(InIteratorT  from, uint fromiWdth, uint fromHeight,
		                OutIteratorT to,   uint toWidth,   uint toHeight);

		/** @brief Block-scale a pixel range.
		 *
		 *  @param from    Begin of the source range
		 *  @param fromEnd End of the source range
		 *  @param to      Begin of the destination range
		 *  @param toEnd   End of the destination range
		 *
		 *  This algorithm block-scales the source range [from, fromEnd] to the
		 *  destination range [to, toEnd].
		 */
		/*template<typename In, typename Out> inline
		void blockScale(In from, In fromEnd, Out to, Out toEnd)
		{
	  }*/

		/** @brief Block-scale an image.
		 *
		 *  @param from      Source image
		 *  @param to        Destination image
		 *  @param newWidth  Wanted width of the destination image
		 *  @param newHeight Wanted height of the destination image
		 */
		template <typename DstColorTagT, typename SrcColorTagT> inline
		void blockScale(const InterleavedImage<SrcColorTagT>& from, InterleavedImage<DstColorTagT>& to,
		                uint newWidth, uint newHeight)
		{
			to.resize(newWidth, newHeight);
			blockScale(from.begin(), from.width(), from.height(), to.begin(), newWidth, newHeight);
		}

	} // namespace Gfx

} // namespace Pt

#endif


//
// Include the implementation header
//
#include "ImageAlgo.tpp"

