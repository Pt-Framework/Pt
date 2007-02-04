/***************************************************************************
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

#include <vector>
//#include <ctime>

#include <Pt/Main.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Algorithm.h>


void ScaleARgbImageTestSimple()
{
	/*const Pt::Gfx::ARgbImage image(30, 20);
	Pt::Gfx::ARgbImage image2;
	blockScale(image, image2, 40, 40);
	*/
}


void ScaleARgbImageTest()
{
	const Pt::Gfx::ARgbImage image(800, 600);
	Pt::Gfx::ARgbImage image2(400, 300);

	//clock_t begin = clock();
	//for(int i = 0; i < 1000; ++i)
	blockScale(image.begin(), image.begin(), image2.end(), image2.end());
	//std::cerr << "PixelIterator: " << clock() - begin << std::endl;
}


void ScaleVectorTest()
{
	std::vector<Pt::Gfx::ARgbColor> from(100*100);
	std::vector<Pt::Gfx::ARgbColor> to(400*300);

	//clock_t begin = clock();
	//for(int i = 0; i < 1000; ++i)
		blockScale(from.begin(), 100, 100, to.begin(), 400, 300);
	//std::cerr << "Manual w/h:" << clock() - begin << std::endl;
}


int main(int argc, char* argv[])
{
	for(int n = 0; n < 100; ++n)
		ScaleARgbImageTest();

	ScaleVectorTest();
	return 0;
}
