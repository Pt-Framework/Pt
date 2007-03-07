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


#include <Pt/Main.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/ARgbColor.h>
#include <Pt/Gfx/ARgbFColor.h>
#include <Pt/Gfx/ARgb8888Color.h>
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb555Color.h>
#include <Pt/Gfx/Rgb565Color.h>
#include <Pt/Gfx/ColorAlgo.h>

//#include <Pt/Gfx/ARgbColorRef.h>

#include <vector>

using namespace Pt;
using namespace Pt::Gfx;


int main( int argc, char* argv[] )
{
/*

    ARgbColor      argb_1, argb_2;
    ARgbFColor     float_1, float_2;
    ARgb8888Color  argb8888_1, argb8888_2;
    Rgb888Color    rgb888_1, rgb888_2;
    Rgb565Color    rgb565_1, rgb565_2;
    Rgb555Color    rgb555_1, rgb555_2;

    const Pt::uint8_t factor = 128;
    blend(argb_1, argb_2, factor);

    argb_2 = argb_1;
    argb_2 = argb8888_1;

    assign(argb_1, argb_2);
    assign(argb_1, argb8888_2);
    assign(argb8888_1, argb_2);

    assign(float_1, argb_1);
    assign(argb_2, float_1);

    greyscale(argb_1, argb_2);
    greyscale(argb_1, argb_1);
    greyscale(argb8888_1);
    greyscale(float_1);

    ////////////////////////////////////////////////////////////////////////////

    typedef std::vector<ColorTraits<ARgbColorProxy>::ComponentT> PlaneT;

    PlaneT ap(320 * 240);
    PlaneT rp(320 * 240);
    PlaneT gp(320 * 240);
    PlaneT bp(320 * 240);

    ARgbColorProxy argb_1_ref(ap[0], rp[0], gp[0], bp[0]);
    ARgbColorProxy argb_2_ref(ap[1], rp[1], gp[1], bp[1]);

    argb_1_ref = argb_1;
    argb_2 = argb_2_ref;
*/
    return 0;
}
