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
#ifndef Pt_Algorithm_tpp
#define Pt_Algorithm_tpp

#include <cmath>


namespace Pt {

	namespace Gfx {

		/*
		template<typename DstColorSpaceT, typename SrcColorSpaceT>
		void old_blockScale(BasicImage<DstColorSpaceT>& dstImage, const BasicImage<SrcColorSpaceT>& srcImage,
		                    uint newWidth, uint newHeight)
		{
			// Resize the destination image
			dstImage.resize(newWidth, newHeight);

			const typename BasicImage<SrcColorSpaceT>::ColorT* src = srcImage.buffer();
			const uint srcW = srcImage.width();
			const uint srcH = srcImage.height();

			typename BasicImage<DstColorSpaceT>::ColorT* dst = dstImage.buffer();
			const uint dstW = dstImage.width();
			const uint dstH = dstImage.height();

			// Perform integer-scan scaling
			uint dh = 0;
			uint y  = 0;
			while(y < dstH) {
				const typename BasicImage<SrcColorSpaceT>::ColorT* sln = src;

				do {
					uint dw = 0;
					for(uint x = 0; x < dstW; ++x) {
						convert(*dst++, *src);
						for(dw += srcW; dw >= dstW; ++src, dw -= dstW);
					}
					src = sln;
					y++;
				} while((dh += srcH) < dstH);

				while(dh >= dstH) {
					src += srcW;
					dh -= dstH;
				}
			}
		}
		*/


		// IMPL: Image scalling using the block scale method (pixel replication/removal)
		template<typename In, typename Out>
		void blockScale(In from, size_t fromWidth, size_t fromHeight,
		                Out to,  size_t toWidth, size_t toHeight)
		{
			size_t dh = 0;
			size_t y  = 0;

			while(y < toHeight) {
				In pos = from;
				do {
					size_t dw = 0;
					for(size_t x = 0; x < toWidth; ++x) {
						assign(*to, *from);
						++to;
						for(dw += fromWidth; dw >= toWidth; ++from, dw -= toWidth);
					}
					from = pos;
					y++;
				}
				while( (dh += fromHeight) < toHeight );

				while(dh >= toHeight) {
					from += fromWidth;
					dh -= toHeight;
				}
			}
		}


		template<typename In, typename Out>
		void blockScale(In from, In fromEnd, Out to, Out toEnd)
		{
			const Math::Size fromSize = fromEnd - from;
			const size_t fromWidth = fromSize.width();
			const size_t fromHeight = fromSize.height();

			const Math::Size toSize = toEnd - to;
			const size_t toWidth = toSize.width();
			const size_t toHeight = toSize.height();

			blockScale(from, fromWidth, fromHeight, to, toWidth, toHeight);
		}

	} // namespace Gfx

} // namespace Pt

#endif

