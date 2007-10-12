/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Duerner                                 *
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
#include <Pt/System/Timer.h>
#include <Pt/System/Selector.h>
#include <iostream>

void onTimer0()
{
    std::cerr << "TIMER-0\n";
}

void onTimer1()
{
    std::cerr << "TIMER-1\n";
}

void onTimeout()
{
    std::cerr << "WAIT-TIMEROUT\n";
}


int main( int argc, char* argv[] )
{
    try
    {
        Pt::System::Timer timer0;
        connect(timer0.timeout, onTimer0);
        timer0.start(2000);

        Pt::System::Timer timer1;
        connect(timer1.timeout, onTimer1);
        timer1.start(1000);

        Pt::System::Selector selector;
        selector.addTimer(timer0);
        selector.addTimer(timer1);
        connect(selector.timeout, onTimeout);

        for(int n = 0; n < 9; ++n)
        {
            selector.wait(500);
        }
    }
    catch( const std::exception& e )
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
