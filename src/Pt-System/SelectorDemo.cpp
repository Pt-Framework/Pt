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
#include <Pt/System/Selector.h>
#include <Pt/System/SerialDevice.h>
#include <fstream>


class Multiplexer : public Pt::Connectable
{
    public:
        Multiplexer()
        : _device("/dev/ttyUSB0", std::ios_base::in)
        //, _device2("COM1:", std::ios_base::in)
        {
            _device.setBaudRate(Pt::System::SerialDevice::BaudRate4800);
            _device.setCharSize(8);
            _device.setStopBits(Pt::System::SerialDevice::OneStopBit);
            _device.setParity(Pt::System::SerialDevice::ParityEven);
            _device.setTimeout( 10 );
/*
            _device2.setBaudRate(Pt::System::SerialDevice::BaudRate1200);
            _device2.setCharSize(7);
            _device2.setStopBits(Pt::System::SerialDevice::OneStopBit);
            _device2.setParity(Pt::System::SerialDevice::ParityEven);
            _device2.setTimeout( 10 );
*/

            //_selector.addDevice( _device, Pt::System::Selector::WaitInput );
            Pt::connect( _device.inputReady, *this, &Multiplexer::onInput );
            Pt::connect( _selector.timeout, *this, &Multiplexer::onTimeout );
        }

        void run()
        {
            for(int i = 0; i < 5000; ++i)
            {
                _selector.wait(100);
            }
        }

        void onTimeout( )
        {
            std::cerr << "--- TIMEOUT ---" << std::endl;
        }

        void onInput()
        {
            char buffer[201];
            memset( buffer, 0, 201);
            size_t size = _device.read( buffer, 200);
            std::cerr.write(buffer, size);
        }

        void onInput2()
        {

        }

    private:
        Pt::System::SerialDevice   _device;
        //Pt::System::SerialDevice _device2;
        Pt::System::Selector       _selector;
        std::ofstream              _out;
};


int main( int argc, char* argv[] )
{
    try
    {
        Multiplexer m;
        m.run();
        std::cerr << "\n\nSUCCESS\n";
    }
    catch( const std::exception& e )
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
