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
#include "Pt/Gfx/YuvColor.h"


namespace Pt {

namespace Gfx {

// YUV->ARGB
//! \todo OPTIMIZE IT !!!
void BasicColor<Yuv>::toARgb(Pt::uint16_t& a, Pt::uint16_t& r, Pt::uint16_t& g, Pt::uint16_t& b) const
{
	a = 32767;

	const float yy = _y / 32767;
	const float uu = _u / 16384;
	const float vv = _v / 16384;
	r = static_cast<int16_t>( 32767 * (1.164f*yy + 1.596f*vv            ) );
	g = static_cast<int16_t>( 32767 * (1.164f*yy - 0.813f*vv - 0.391f*uu) );
	b = static_cast<int16_t>( 32767 * (1.164f*yy             + 2.018f*uu) );
}


// ARGB->YUV
//! \todo OPTIMIZE IT !!!
void BasicColor<Yuv>::fromARgb(uint16_t a, uint16_t r, uint16_t g, uint16_t b)
{
	const float rr = static_cast<float>(r) * a / 32767 / 32767;
	const float gg = static_cast<float>(g) * a / 32767 / 32767;
	const float bb = static_cast<float>(b) * a / 32767 / 32767;

	setLuminance   ( static_cast<int16_t>( 32767 * ( (0.257f*rr) + (0.504f*gg) + (0.098f*bb)) ) );
	setChrominanceU( static_cast<int16_t>( 16384 * (-(0.148f*rr) - (0.291f*gg) + (0.439f*bb)) ) );
	setChrominanceV( static_cast<int16_t>( 16384 * ( (0.439f*rr) - (0.368f*gg) - (0.071f*bb)) ) );
}


bool BasicColor<Yuv>::lt(const BasicColor<ColorSpaceT>& c) const
{
	if(_y < c._y) return true;
	if(_u < c._u) return true;
	if(_v < c._v) return true;
	return false;
}


bool BasicColor<Yuv>::gt(const BasicColor<ColorSpaceT>& c) const
{
	if(_y > c._y) return true;
	if(_u > c._u) return true;
	if(_v > c._v) return true;
	return false;
}

} // namespace Gfx

} // namespace Pt
