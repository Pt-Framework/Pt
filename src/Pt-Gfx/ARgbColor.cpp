/***************************************************************************
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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
#include <Pt/Gfx/ARgbColor.h>
#include <iomanip>
#include <sstream>

using namespace std;


namespace Pt {

namespace Gfx {

bool BasicColor<ARgb>::lt(const BasicColor<ColorSpaceT>& c) const
{
	if(_a < c._a) return true;
	if(_r < c._r) return true;
	if(_g < c._g) return true;
	if(_b < c._b) return true;
	return false;
}


bool BasicColor<ARgb>::gt(const BasicColor<ColorSpaceT>& c) const
{
	if(_a > c._a) return true;
	if(_r > c._r) return true;
	if(_g > c._g) return true;
	if(_b > c._b) return true;
	return false;
}

//! \todo OPTIMIZE IT !!!
void BasicColor<ARgb>::setBrightness(uint16_t l)
{
	if(l <= 0) {
		_r = _g = _b = 0;
		return;
	}

	if(_r>=_g && _r>=_b) {
		const float o = _r;
		_r  = l;
		_g *= static_cast<uint16_t>(l/o);
		_b *= static_cast<uint16_t>(l/o);
	}
	else if (_g>=_r && _g>=_b) {
		const float o = _g;
		_r *= static_cast<uint16_t>(l/o);
		_g  = l;
		_b *= static_cast<uint16_t>(l/o);
	}
	else {
		const float o = _b;
		_r *= static_cast<uint16_t>(l/o);
		_g *= static_cast<uint16_t>(l/o);
		_b  = l;
	}
}

} // namespace Gfx

} // namespace Pt
