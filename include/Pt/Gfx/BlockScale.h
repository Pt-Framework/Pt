/* Copyright (C) 2010-2016 Marc Boris Duerner
   Copyright (C) 2006-2010 by Aloysius Indrayanto

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_BLOCKSCALE_H
#define PT_GFX_BLOCKSCALE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Types.h>
#include <cstddef>


namespace Pt {
namespace Gfx {


/** @brief Generic block scale implementation.
*/
template<typename InIterT, typename OutIterT>
void blockScale(InIterT from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
                OutIterT to, Pt::ssize_t toWidth, Pt::ssize_t toHeight)
{
    Pt::ssize_t dh = 0;
    Pt::ssize_t y  = 0;

    while(y < toHeight)
    {
        InIterT pos = from;
        do
        {
            Pt::ssize_t dw = 0;
            for(Pt::ssize_t x = 0; x < toWidth; ++x)
            {
                *to = *from;
                ++to;

                for(dw += fromWidth; dw >= toWidth; ++from, dw -= toWidth)
                    ;
            }

            from = pos;
            y++;
        }
        while( (dh += fromHeight) < toHeight );

        while(dh >= toHeight)
        {
            from += fromWidth;
            dh -= toHeight;
        }
    }
}


/** @brief Generic block scale implementation with a custom assignment function..
*/
template<typename InIterT, typename OutIterT, typename AssignT>
void blockScale(InIterT from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
                OutIterT to, Pt::ssize_t toWidth, Pt::ssize_t toHeight,
                AssignT assign)
{
    Pt::ssize_t dh = 0;
    Pt::ssize_t y  = 0;

    while(y < toHeight)
    {
        InIterT pos = from;
        do
        {
            Pt::ssize_t dw = 0;
            for(Pt::ssize_t x = 0; x < toWidth; ++x)
            {
                assign(*to, *from);
                ++to;

                for(dw += fromWidth; dw >= toWidth; ++from, dw -= toWidth)
                    ;
            }

            from = pos;
            y++;
        }
        while( (dh += fromHeight) < toHeight );

        while(dh >= toHeight)
        {
            from += fromWidth;
            dh -= toHeight;
        }
    }
}


} // namespace
} // namespace

#endif
