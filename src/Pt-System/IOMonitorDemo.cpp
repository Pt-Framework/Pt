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

#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>


class Multiplexer : public Pt::System::Thread, public Pt::Connectable
{
    public:
        void exit()
        {
            _eloop.exit();
            this->wait();
        }

    protected:
        void run()
        {
            _device = new Pt::System::SerialDevice("/dev/ttyUSB0", std::ios_base::in);
            _device->setBaudRate(Pt::System::SerialDevice::BaudRate4800);
            //_device->setCanonical( 10 );
            //_device->disableCanonical();
            _device->setCharSize(8);
            _device->setStopBits(Pt::System::SerialDevice::OneStopBit);
            _device->setParity(Pt::System::SerialDevice::ParityEven);

                    //_device->setCanonical( 'G' );
                        
            Pt::Signal<const Pt::System::IOEvent&>& signal = _eloop.addDevice( *_device );
            Pt::connect( signal, *this, &Multiplexer::onIOEvent );
            //Pt::System::Thread::sleep(800);

            _eloop.run();
        }

        void onIOEvent( const Pt::System::IOEvent& ev )
        {
            const Pt::System::ReadEvent* readEvent = 0;
            readEvent = dynamic_cast<const Pt::System::ReadEvent*>( &ev );
            if( readEvent != 0 )
            {
                char buffer[201];
                memset( buffer, 0, 201);
                size_t size = 0;

                if( size = _device->read( buffer, 200) )
                {
                    //std::cerr<<"Read "<<size<<"(bytes):";
                    std::cerr.write(buffer, size);
                    std::cerr << std::endl;
                    
                    //if(size == 1)
                        //std::cerr << (int)buffer[0];
                }
            }
        }

    private:
        Pt::System::SerialDevice* _device;
        Pt::System::EventLoop _eloop;
};


int main( int argc, char* argv[] )
{
    try
    {
        Multiplexer m;
        m.start();

        Pt::System::Thread::sleep( 5000 );

        m.exit();
    }
    catch( const std::exception& e )
    {
        std::cerr<<e.what()<<std::endl;
    }

    return 0;
}
