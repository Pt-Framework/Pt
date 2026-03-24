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
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Gfx/LineView.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

/** @brief Block scale from a source view to a target view.
*/
template <typename FromView, typename ToView>
void blockScale(const FromView& from, ToView& to)
{
    const Pt::ssize_t fromWidth  = from.width();
    const Pt::ssize_t fromHeight = from.height();
    const Pt::ssize_t toWidth    = to.width();
    const Pt::ssize_t toHeight   = to.height();

    auto fromLines = lineView(from);
    auto toLines   = lineView(to);
    auto fromLine  = fromLines.begin();
    auto toLine    = toLines.begin();
    auto toLineEnd = toLines.end();

    Pt::ssize_t dh = 0;

    while(toLine != toLineEnd)
    {
        Pt::ssize_t rows = (toHeight - dh + fromHeight - 1) / fromHeight;

        for(Pt::ssize_t i = 0; i < rows; ++i)
        {
            auto fromIt = fromLine->begin();

            Pt::ssize_t dw = 0;
            for(auto& toPixel : *toLine)
            {
                copyPixel(*fromIt, toPixel);

                dw += fromWidth;
                fromIt += dw / toWidth;
                dw %= toWidth;
            }

            ++toLine;
        }

        dh += rows * fromHeight;
        fromLine += dh / toHeight;
        dh %= toHeight;
    }
}

} // namespace

} // namespace

#endif
