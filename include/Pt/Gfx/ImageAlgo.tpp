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
#ifndef Pt_Gfx_ImageAlgo_tpp
#define Pt_Gfx_ImageAlgo_tpp


namespace Pt {

namespace Gfx {

    template <typename DstColorT, typename SrcColorT>
    void assign(InterleavedImage<DstColorT>& to, const InterleavedImage<SrcColorT>& from)
    {
        if(from.empty()) {
            to.clear();
            return;
        }

        if( to.width() != from.width() || to.height() != from.height() )
            to.resize( from.width(), from.height() );

        for(uint y = 0; y < to.height(); y++)
            for(uint x = 0; x < to.width(); x++)
                assign(to.pixel(x, y), from.pixel(x, y)); // TODO: Optimize it !!!
    }


    template<typename InIteratorT, typename OutIteratorT>
    void blockScale(InIteratorT  from, uint fromWidth, uint fromHeight,
                    OutIteratorT to,   uint toWidth,   uint toHeight)
    {
        size_t dh = 0;
        size_t y  = 0;

        while(y < toHeight) {
            InIteratorT pos = from;
            do {
                size_t dw = 0;
                for(size_t x = 0; x < toWidth; ++x) {
                    //assign(*to, *from);
                    *to = *from;
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

} // namespace Gfx

} // namespace Pt

#endif
