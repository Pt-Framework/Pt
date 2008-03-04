/***************************************************************************
 *   Copyright (C) 2007-2008 Marc Boris Duerner                            *
 *   Copyright (C) 2007 Laurentiu-Gheorghe Crisan                          *
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
#include <Pt/System/EventLoop.h>
#include <Pt/System/Timer.h>
#include <Pt/System/Pipe.h>
#include <iostream>

Pt::System::EventLoop loop;
Pt::System::Pipe mypipe(Pt::System::IODevice::Async);
char buffer[255];

void onRead(Pt::System::IOResult& result)
{
    std::size_t n = result.device()->endRead(result);
    Pt::System::IOResult& readResult = mypipe.input().beginRead(buffer, 4);
    loop.add(readResult);

    std::cout.write(buffer, n) << std::endl;
}

int main( int argc, char* argv[] )
{
    try
    {
        mypipe.output().write("Hello World!", 12);

        connect(mypipe.input().inputReady, &onRead);
        Pt::System::IOResult& readResult = mypipe.input().beginRead(buffer, 4);

        Pt::System::Timer stopper;
        stopper.start(2000);
        connect(stopper.timeout, loop, &Pt::System::EventLoop::exit);

        loop.add(stopper);
        loop.add(readResult);
        loop.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl; 
    }
}
