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
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <fstream>

class SerialListener : public Pt::Connectable
{
    public:    
        SerialListener( Pt::System::SerialDevice& device)
        : _device ( device )
        , _out( "out.txt")
        { 
        }
        
        ~SerialListener()
        { }
        
        void serialEvent( const Pt::System::IOEvent& ev )
        {
            std::cerr<< "Event: "<< typeid(ev).name()<<std::endl;
            _out<< "Event: "<< typeid(ev).name()<<std::endl;
            
            const Pt::System::ReadEvent* readEvent = dynamic_cast<const Pt::System::ReadEvent*>( &ev );
            
            if( readEvent != 0 )
            {
                char buffer[201];
                memset( buffer, 0, 201);
                size_t size = 0;
                
                while( size = _device.read( buffer, 200) )
                {
                    std::cerr << "Read: " << buffer << " (" << size << " bytes)" << std::endl;
                    _out<< "Read: " << buffer << " (" << size << " bytes)" << std::endl;
                }
            }
            else
            {            
                const Pt::System::WriteEvent* writeEvent = dynamic_cast<const Pt::System::WriteEvent*>( &ev );
                
                if(writeEvent != 0)
                {
                    std::cerr<<"Data transmission complete."<<std::endl;
                    _out<<"Data transmission complete."<<std::endl;
                }
            }
        }  
        
        void printMouseId()
        {
            char buffer[201];
            char byte;
            memset( buffer, 0, 201);
          
            std::cerr << "Reading bytes " << std::endl;
            size_t size = _device.read( buffer, 200);
            std::cerr << "Read: " << buffer << " (" << size << " bytes)" << std::endl;

            std::cerr << "Parsing PnP data " << std::endl;
            std::string pnpString;
            for( size_t i = 0; i < size; i++)
            {
              byte = buffer[i];
              if(byte == 0x08 || byte == 0x28) 
              {
                int offset = 0x28 - byte;
                int stop = byte + 1;
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

            std::cerr << pnpString << std::endl;        
        }
              
    private:
        enum{ BufferSize = 200 };
        
        Pt::System::SerialDevice&   _device;
        char                        _buffer[BufferSize];
        std::ofstream               _out;
};

int main( int argc, char* argv[] )
{    
    try
    {
        Pt::System::Thread::sleep(20000);
        Pt::System::EventLoop       eventLoop;
        Pt::System::Thread          thread( eventLoop ); 
        Pt::System::SerialDevice    serialDevice("COM5:", std::ios_base::in | std::ios_base::out);
       
        //Setup the device         
        serialDevice.setBaudRate(Pt::System::SerialDevice::BaudRate9600);
        serialDevice.setCharSize(8);
        serialDevice.setStopBits(Pt::System::SerialDevice::OneStopBit);
        serialDevice.setParity(Pt::System::SerialDevice::ParityNone);
        
        //Create a device listener 
        SerialListener listener( serialDevice );                
        
        //Add a device to the event loop.
        Pt::Signal<const Pt::System::IOEvent&>& signal = eventLoop.addDevice( serialDevice );        
                
        //Connect the device listener to the device.
        Pt::connect( signal, listener, &SerialListener::serialEvent );

        //Start the loop.
        thread.start();
        
        //Trigger the serial flags for plag&play mouse.
        serialDevice.setFlowControl(Pt::System::SerialDevice::FlowControlHard);
        Pt::System::Thread::sleep( 300 );
        serialDevice.setFlowControl(Pt::System::SerialDevice::FlowControlSoft);
        Pt::System::Thread::sleep( 300 );
        
        //Wait a time periode. 
        Pt::System::Thread::sleep(  5000 );
        
        //Write something.
        char* buffer = new char[100];
        memset( buffer, 23, 100 );
        size_t no = serialDevice.write( buffer, 2 );
        delete []buffer;
        
        //serialDevice.flush();
        
        //Wait again.
        Pt::System::Thread::sleep(  1000 );
        
        //Exit the event loop.
        eventLoop.exit();

        //Remove the serial device. 
        eventLoop.removeDevice( serialDevice );

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
