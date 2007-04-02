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
#include <Pt/System/IOMonitor.h>

void onTimer()
{
    std::cerr << "TIMER\n";
}


void onTimeout()
{
    std::cerr << "WAIT-TIMEROUT\n";
}


int main( int argc, char* argv[] )
{
    try
    {
        Pt::System::Timer timer;
        connect(timer.timeout, onTimer);
        timer.start(2000);

        Pt::System::IOMonitor monitor;
        monitor.addTimer(timer);
        connect(monitor.timeout, onTimeout);

        while(true)
            monitor.wait(500);
    }
    catch( const std::exception& e )
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
