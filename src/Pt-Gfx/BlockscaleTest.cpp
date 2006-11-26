/***************************************************************************
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

#include <vector>

#include <Pt/Main.h>
#include <Pt/Gfx/ARgbColor.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Algorithm.h>


void ScaleARgbImageTest()
{
	const Pt::Gfx::ARgbImage image(30, 20);
	Pt::Gfx::ARgbImage image2(40, 40);
	blockScale(image.begin(), image.end(), image2.begin(), image2.end());
}


void ScaleVectorTest()
{
	std::vector<Pt::Gfx::ARgbColor> from(30*20);
	std::vector<Pt::Gfx::ARgbColor> to(40*30);
	blockScale(from.begin(), 30, 20, to.begin(), 40, 30);
}


int main(int argc, char* argv[])
{
	ScaleARgbImageTest();
	ScaleVectorTest();
	return 0;
}
