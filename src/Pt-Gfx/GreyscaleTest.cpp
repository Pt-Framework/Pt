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
