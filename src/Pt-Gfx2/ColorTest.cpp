/***************************************************************************
 *   Copyright (C) 2006 Aloysius Indrayanto                                *
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
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

#include <Pt/Gfx2/ARgbColor.h>
#include <Pt/Gfx2/ARgbFColor.h>
#include <Pt/Gfx2/ARgb8888Color.h>
#include <Pt/Gfx2/ColorAlgo.h>

using namespace Pt;
using namespace Pt::Gfx;


int main()
{
	ARgbColor      argb_1, argb_2;
	ARgbFColor     float_1, float_2;
	ARgb8888Color  argb8888_1, argb8888_2;

	Pt::uint16_t a, r, g, b;

	toARgb(a, r, g, b, argb_1);
	fromARgb(argb_2, a, r, g, b);

	toARgb(a, r, g, b, argb8888_1);
	fromARgb(argb8888_2, a, r, g, b);

	assign(argb_1, argb_2);
	assign(argb_1, argb8888_2);
	assign(argb8888_1, argb_2);

	assign(float_1, argb_1);
	assign(argb_2, float_1);

	greyscale(argb_1, argb_2);
	greyscale(argb_1, argb_1);
	greyscale(argb8888_1);
	greyscale(float_1);
}
