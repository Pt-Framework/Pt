/* Copyright (C) 2015-2017 Marc Boris Duerner
   Copyright (C) 2016-2017 Aloysius Indrayanto

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

#include "Argb32SIMDOps.h"
#include <Pt/Gfx/Argb32Image.h>

namespace Pt {

namespace Gfx {

void Argb32Model::assign(Pt::uint8_t* to, const Color& c, size_t length,
                         CompositionMode mode)
{
    switch(mode) {
        default:
        case CompositionMode::SourceCopy: {
            const Pt::uint32_t src = ( Pt::uint32_t(c.alpha() & 0xFF00) << 16 ) |
                                      ( Pt::uint32_t(c.red  () & 0xFF00) <<  8 ) |
                                      ( Pt::uint32_t(c.green() & 0xFF00)       ) |
                                      ( Pt::uint32_t(c.blue ()         ) >>  8 );
            Argb32::pixelOps_SourceCopy(to, src, length);
            break;
        }

        case CompositionMode::SourceOver: {
            const Pt::uint32_t blend    = c.alpha() >> 8;
            const Pt::uint32_t blendInv = 255 - blend;
            const Pt::uint32_t srcR     = (Pt::uint32_t) (c.red  () >> 8) * blend;
            const Pt::uint32_t srcG     = (Pt::uint32_t) (c.green() >> 8) * blend;
            const Pt::uint32_t srcB     = (Pt::uint32_t) (c.blue () >> 8) * blend;
            const Pt::uint32_t srcA     = blend * blend;
            Argb32::pixelOps_SourceOver(to, srcA, srcR, srcG, srcB, blendInv, length);
            break;
        }
    }
}

void Argb32Model::assign(Pt::uint8_t* to, const Pt::uint8_t* from, size_t length,
                         CompositionMode mode)
{
    switch(mode) {
        default:
        case CompositionMode::SourceCopy: {
            const Pt::uint32_t src = *reinterpret_cast<const Pt::uint32_t*>(from);
            Argb32::pixelOps_SourceCopy(to, src, length);
            break;
        }

        case CompositionMode::SourceOver: {
            const Pt::uint32_t blend    = from[3];
            const Pt::uint32_t blendInv = 255 - blend;
            const Pt::uint32_t srcR     = from[2] * blend;
            const Pt::uint32_t srcG     = from[1] * blend;
            const Pt::uint32_t srcB     = from[0] * blend;
            const Pt::uint32_t srcA     = blend   * blend;
            Argb32::pixelOps_SourceOver(to, srcA, srcR, srcG, srcB, blendInv, length);
            break;
        }
    }
}

} // namespace

} // namespace
