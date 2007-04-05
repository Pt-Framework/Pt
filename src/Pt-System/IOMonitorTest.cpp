/***************************************************************************
 *   Copyright (C) 2007 by Laurentiu-Gheorghe Crisan                       *
 *   Copyright (C) 2007 by Marc Boris Dürner                               *
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
#include "Pt/System/SerialDevice.h"
#include "Pt/System/EventLoop.h"
#include "Pt/System/Thread.h"
#include "Pt/System/ReadEvent.h"
#include "Pt/System/WriteEvent.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>

class SerialDeviceListener : public Pt::Connectable
{
    public:

        SerialDeviceListener( Pt::System::SerialDevice& device )
        : _device( device )
        , _hasReadEvent( false )
        , _hasData( false )
        { }

        ~SerialDeviceListener()
        { }

        bool hasReadEvent() const
        { return _hasReadEvent; }
        
        bool hasData() const
        { return _hasData; }
        
        void serialEvent( const Pt::System::IOEvent& ev )
        {
            const Pt::System::ReadEvent* readEvent = dynamic_cast<const Pt::System::ReadEvent*>( &ev );
            
            if( readEvent != 0 )
            {
                _hasReadEvent = true;
                printMouseId();
            }
            else
            {
                const Pt::System::WriteEvent* writeEvent = dynamic_cast<const Pt::System::WriteEvent*>( &ev );

                if( writeEvent != 0 )
                { }
            }
        }
        
    private:
    
        void printMouseId()
        {
            char    buffer[201];
            char    byte;
            
            memset( buffer, 0, 201);

            size_t size = _device.read( buffer, 1);

            if( (int)buffer[0] == 0 )
                return;

            size = _device.read( buffer, 200);

            std::string pnpString;
            
            for( size_t i = 0; i < size; i++ )
            {
                byte = buffer[i];
                
                if( byte == 0x08 || byte == 0x28 )
                {
                    int offset  = 0x28 - byte;
                    int stop    = byte + 1;
                    i++;
                    
                    for( ; i < size; i++ )
                    {
                        byte = buffer[i];
                        
                        if(byte == stop)
                            break;

                        byte += offset;
                        pnpString.append(1, byte);
                    }
                    
                    break;
                }
            }

            if( !_hasData )
                _hasData  = !pnpString.empty();
        }

        Pt::System::SerialDevice&   _device;
        bool                        _hasReadEvent;
        bool                        _hasData;
};

class IOMonitorTest : public Pt::Unit::TestSuite
{
    public:
        IOMonitorTest()
        : Pt::Unit::TestSuite("IOMonitorTest")
        {
            Pt::Unit::TestSuite::registerMethod( "test", *this, &IOMonitorTest::test );
        }

    private:
        void test()
        {
            #if defined(WIN32) || defined(_WIN32)
                std::string port("COM1:");
            #else
                std::string port("/dev/ttyS0");
            #endif

            try
            {
                Pt::System::EventLoop       eventLoop;
                Pt::System::Thread          thread( eventLoop );
                Pt::System::SerialDevice    serialDevice( port, std::ios_base::in | std::ios_base::out );

                //Setup the device.
                serialDevice.setBaudRate(Pt::System::SerialDevice::BaudRate1200);
                serialDevice.setCharSize(7);
                serialDevice.setStopBits(Pt::System::SerialDevice::OneStopBit);
                serialDevice.setParity(Pt::System::SerialDevice::ParityNone);
                serialDevice.setFlowControl(Pt::System::SerialDevice::FlowControlHard);

                //Create a device listener.
                SerialDeviceListener listener( serialDevice );

                //Add a device to the event loop.
                Pt::Signal<const Pt::System::IOEvent&>& signal = eventLoop.addDevice( serialDevice, Pt::System::IODevice::WaitInput );

                //Connect the device listener to the device.
                Pt::connect( signal, listener, &SerialDeviceListener::serialEvent );

                //Start the loop.
                thread.start();

                //Trigger the serial flags for plag&play mouse.
                serialDevice.setFlowControl( Pt::System::SerialDevice::FlowControlHard );
                Pt::System::Thread::sleep( 300 );
                serialDevice.setFlowControl( Pt::System::SerialDevice::FlowControlSoft );
                Pt::System::Thread::sleep( 300 );

                //Wait a time periode.
                Pt::System::Thread::sleep(  500 );

                //Exit the event loop.
                eventLoop.exit();

                //Remove the serial device.
                eventLoop.removeDevice( serialDevice );

                //Close the device
                serialDevice.close();

                //Join the threads.
                thread.wait();
                
                //Check.
                PT_UNIT_ASSERT( listener.hasReadEvent() == true );
               
                PT_UNIT_ASSERT( listener.hasData() == true );
            }
            catch( const Pt::System::OpenFailed f )
            {
                f.what();
                
                message( "No device found for executing the test.\n\
                          Plug in a serial mouse on the first port and try again." );
            }
        }
};

Pt::Unit::RegisterTest<IOMonitorTest> register_IOMonitorTest;
