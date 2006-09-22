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
#include <Pt/Exception.h>
#include <Pt/Gfx/CmykColor.h>


namespace Pt {

namespace Gfx {

//! \todo OPTIMIZE IT !!!
void Pt::Gfx::BasicColor<Cmyk>::toARgb(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b) const
{
	int32_t d = 0xFFFF - _k;
	int32_t c = _c*d +_k;
	int32_t m = _m*d +_k;
	int32_t y = _y*d +_k;

	a = 0xFFFF;
	r = 0xFFFF - c;
	g = 0xFFFF - m;
	b = 0xFFFF - y;
}


//! \todo OPTIMIZE IT !!!
void Pt::Gfx::BasicColor<Cmyk>::fromARgb(uint16_t a, uint16_t r, uint16_t g, uint16_t b)
{
	const int32_t rr = static_cast<int32_t>(r) * a / 0xFFFF;
	const int32_t gg = static_cast<int32_t>(g) * a / 0xFFFF;
	const int32_t bb = static_cast<int32_t>(b) * a / 0xFFFF;

	uint16_t c = 0xFFFF - rr;
	uint16_t m = 0xFFFF - gg;
	uint16_t y = 0xFFFF - bb;
	uint16_t k = 0xFFFF;

	if(c < k) k = c;
	if(m < k) k = m;
	if(y < k) k = y;

	if(k == 0xFFFF) {
		_c = 0;
		_m = 0;
		_y = 0;
		_k = 0xFFFF;
	}
	else {
		const float kk = 0xFFFF - k;
		const float dd = 0xFFFF / kk;

		setCyan   (static_cast<uint16_t>( (c-k) * dd ));
		setMagenta(static_cast<uint16_t>( (m-k) * dd ));
		setYellow (static_cast<uint16_t>( (y-k) * dd )) ;
		setBlack  (k);
	}
}


bool Pt::Gfx::BasicColor<Cmyk>::lt(const BasicColor<ColorSpaceT>& c) const
{
	if(_c < c._c) return true;
	if(_m < c._m) return true;
	if(_y < c._y) return true;
	if(_k < c._k) return true;
	return false;
}


bool Pt::Gfx::BasicColor<Cmyk>::gt(const BasicColor<ColorSpaceT>& c) const
{
	if(_c > c._c) return true;
	if(_m > c._m) return true;
	if(_y > c._y) return true;
	if(_k > c._k) return true;
	return false;
}

} // namespace Gfx

} // namespace Pt
