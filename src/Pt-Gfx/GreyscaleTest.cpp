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
#include <iostream>
#include <sstream>
#include <ctime>

using namespace std;


int main(int argc, char* argv[])
{
    /*std::vector<Pt::Gfx::ARgbColor> data(1000);

    clock_t begin, time;

    begin = clock();
    for(int i = 0; i < 10000; ++i)
    {
        Pt::Gfx::transform( data.begin(), data.end(), Pt::Gfx::Greyscale<Pt::Gfx::ARgb>() );
    }
    time = clock() - begin;

    std::cerr << "Duration: " << time << std::endl;

    begin = clock();
    for(int i = 0; i < 10000; ++i)
    {
        Pt::Gfx::greyscale( data.begin(), data.end() );
    }
    time = clock() - begin;

    std::cerr << "Duration: " << time << std::endl;*/

    return 0;
}
