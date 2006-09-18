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
#include <Pt/Gfx/Rgb565Color.h>
using namespace Pt;
using namespace Pt::Gfx;


Pt::uint8_t Pt::Gfx::BasicColor<Rgb565>::brightness() const
{
	Pt::uint8_t r = red();
	Pt::uint8_t g = green();
	Pt::uint8_t b = blue();
	return((r>=g && r>=b) ? r : ((g>=r && g>=b) ? g : b));
}


void Pt::Gfx::BasicColor<Rgb565>::setBrightness(Pt::uint8_t l)
{
	if(l == 0) {
		_val = 0;
		return;
	}

	Pt::uint8_t r = red();
	Pt::uint8_t g = green();
	Pt::uint8_t b = blue();

	if(r>=g && r>=b) {
		float o = r;
		setRed  (l);
		setGreen(Pt::uint8_t(g * l / o));
		setBlue (Pt::uint8_t(b * l / o));
	}
	else if (g>=r && g>=b) {
		float o = g;
		setRed  (Pt::uint8_t(r * l / o));
		setGreen(l);
		setBlue (Pt::uint8_t(b * l / o));
	}
	else {
		float o = b;
		setRed  (Pt::uint8_t(r * l / o));
		setGreen(Pt::uint8_t(g * l / o));
		setBlue (l);
	}
}
