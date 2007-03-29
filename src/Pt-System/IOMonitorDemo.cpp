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
#include <Pt/System/SerialDevice.h>
#include <Pt/System/IOEvent.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Thread.h>
#include <Pt/System/IODevice.h>
#include <Pt/System/ReadEvent.h>
#include <Pt/System/WriteEvent.h>
#include <Pt/System/EventSource.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <fstream>


class Multiplexer : public Pt::Connectable
{
    public:
        Multiplexer()
        : _device("/dev/ttyUSB0", std::ios_base::in)
        {
            _device.setBaudRate(Pt::System::SerialDevice::BaudRate4800);
            _device.setCharSize(8);
            _device.setStopBits(Pt::System::SerialDevice::OneStopBit);
            _device.setParity(Pt::System::SerialDevice::ParityEven);

            Pt::Signal<const Pt::System::IOEvent&>& signal = _monitor.addDevice( _device );
            Pt::connect( signal, *this, &Multiplexer::onIOEvent );
        }

        void run()
        {
            for(int i = 0; i < 10000; ++i)
            {
                if( !_monitor.wait(200) )
                    std::cerr << "--- NO DATA ---" << std::endl;
            }
        }

        void onIOEvent( const Pt::System::IOEvent& ev )
        {
            const Pt::System::ReadEvent* readEvent = 0;
            readEvent = dynamic_cast<const Pt::System::ReadEvent*>( &ev );
            if( readEvent != 0 )
            {
                char buffer[201];
                memset( buffer, 0, 201);
                size_t size = _device.read( buffer, 200);
                std::cerr.write(buffer, size);
            }
        }

    private:
        Pt::System::SerialDevice _device;
        Pt::System::IOMonitor _monitor;
};


int main( int argc, char* argv[] )
{
    try
    {
        Multiplexer m;
        m.run();
    }
    catch( const std::exception& e )
    {
        std::cerr<<e.what()<<std::endl;
    }

    return 0;
}
