/*
 * Copyright (C) 2005 by Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Pt/Main.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Algorithm.h>
#include <vector>
//#include <ctime>


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
