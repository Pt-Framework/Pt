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

class SerialListener : public Pt::Connectable
{
    public:
        SerialListener( Pt::System::SerialDevice& device)
        : _device ( device )
        , _out("ser.txt")
        {         
            //The event source is a thread save sender.
            //We connect us to the event source as the second consumer.
            _eventSource.connect(  slot( this, &SerialListener::theSecondConsumer ).clone() );
        }

        ~SerialListener()
        { }

        void serialEvent( const Pt::System::IOEvent& ev )
        {
            //Check the event type.( read/write? ).
            const Pt::System::ReadEvent* readEvent = dynamic_cast<const Pt::System::ReadEvent*>( &ev );

            if( readEvent != 0 )
            {
                char buffer[201];
                memset( buffer, 0, 201);
                size_t size = 0;

                if( size = _device.read( buffer, 200) )
                {
                    _out<<"Size= "<<size<<"(bytes)"<<std::endl;
                    _out<<"Data: " << buffer << std::endl; 
                    
                    if( size == 1 )
                        _out<<"Char= "<<(int) buffer[0]<<std::endl;

                    //std::cerr<<"---"<<std::endl;
                    memset( buffer, 0, 200);
                }
            }
            else
            {
                const Pt::System::WriteEvent* writeEvent = dynamic_cast<const Pt::System::WriteEvent*>( &ev );

                if( writeEvent != 0 )
                    _out<<"Data transmission complete."<<std::endl;
            }

            //Send the event to the second consumer.
            //_eventSource.send( ev );
        }

        void theSecondConsumer( const Pt::System::IOEvent& ev )
        {
            std::cerr<< "Second consumer of the event: "<< typeid(ev).name()<<std::endl;
        }

    private:
        Pt::System::SerialDevice&   _device;
        Pt::System::EventSource     _eventSource;        
        std::ofstream  _out;
};

int main( int argc, char* argv[] )
{
    try
    {      
        Pt::System::Thread::sleep( 20000 );  
        //The event loop agregates an IOMonitor.
        Pt::System::EventLoop    eventLoop;

        //Pack the event loop in a thread.
        Pt::System::Thread       thread( eventLoop );

        //Setup a serial device.
        Pt::System::SerialDevice serialDevice("COM5:", std::ios_base::in);
        serialDevice.setBaudRate(Pt::System::SerialDevice::BaudRate4800);
        serialDevice.setCanonical( 10 );
        serialDevice.setCharSize(7);
        serialDevice.setStopBits(Pt::System::SerialDevice::OneStopBit);
        serialDevice.setParity(Pt::System::SerialDevice::ParityEven);


        //Create a device listener
        SerialListener listener( serialDevice );

        //Add the serial device to the event loop.
        Pt::Signal<const Pt::System::IOEvent&>& signal = eventLoop.addDevice( serialDevice );

        //Connect the device listener to the serial device.
        Pt::connect( signal, listener, &SerialListener::serialEvent );

        //Start the loop.
        thread.start();

        //Wait a time periode.
        Pt::System::Thread::sleep( 10000 );

        //Write something.
        char buffer[100];
        memset( buffer, 23, 100 );
        //size_t no = serialDevice.write( buffer, 100 );

        //Wait again.
        Pt::System::Thread::sleep( 1000 );

        //Exit the event loop.
        eventLoop.exit();

        //Remove the serial device from the event loop.
        eventLoop.removeDevice( serialDevice );

        //Close teh serial device.
        serialDevice.close();

        //Join the threads.
        thread.wait();
    }
    catch( const std::exception& e )
    {
        std::cerr<<e.what()<<std::endl;
    }

    return 0;
}
