#include <Pt/Main.h>
#include <Pt/System/SerialDevice.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Selector.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "SerialDeviceImpl.h"


void readMousePnp()
{
    std::string port("/dev/ttyS0");
    //std::string port("COM1:");
    std::cerr << "Opening " << port << std::endl;
    Pt::System::SerialDevice serdev( port,  std::ios_base::in );

    std::cerr << "Setting baud rate " << std::endl;
    serdev.setBaudRate(Pt::System::SerialDevice::BaudRate1200);

    std::cerr << "Setting char size " << std::endl;
    serdev.setCharSize(7);

    std::cerr << "Setting stop bits " << std::endl;
    serdev.setStopBits(Pt::System::SerialDevice::OneStopBit);

    std::cerr << "Setting stop bits " << std::endl;
    serdev.setParity(Pt::System::SerialDevice::ParityNone);

    std::cerr << "Setting flow control" << std::endl;
    serdev.setFlowControl(Pt::System::SerialDevice::FlowControlHard);
    Pt::System::Thread::sleep( 300 );

    serdev.setFlowControl(Pt::System::SerialDevice::FlowControlSoft);
    Pt::System::Thread::sleep( 300 );

    char buffer[201];
    char byte;
    memset( buffer, 0, 201);

    std::cerr << "Reading bytes " << std::endl;
    size_t size = serdev.read( buffer, 1);
    std::cerr << "Read: " << (int) buffer[0] << " (" << size << " bytes)" << std::endl;

    if( (int)buffer[0] == 0 )
        return;

    std::cerr << "Reading bytes " << std::endl;
    size = serdev.read( buffer, 200);
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



int main( int argc, char* argv[] )
{
    try
    {
        const int size = 300;
        char buffer[size];

        //std::string port = "/dev/ttyS0";
        std::string port = "COM8:";
        std::cerr << "Opening " << port << std::endl;
        Pt::System::SerialDevice serialDevice(port, std::ios_base::in);
        serialDevice.setBaudRate(Pt::System::SerialDevice::BaudRate4800);
        serialDevice.setCharSize(8);
        serialDevice.setStopBits(Pt::System::SerialDevice::OneStopBit);
        serialDevice.setParity(Pt::System::SerialDevice::ParityNone);
        serialDevice.setFlowControl(Pt::System::SerialDevice::FlowControlHard);
        serialDevice.setTimeout(100);

        Pt::System::Selector selector;

        size_t count = 0;

        DWORD waitMask = 0;

 
    DWORD length;

        DWORD waitCommMask = EV_BREAK | EV_RXCHAR | EV_RXFLAG;
        SetCommMask( ((Pt::System::SerialDeviceImpl*)serialDevice.impl())->_handle, waitCommMask );    

        while(true)
        {
           /* Pt::System::IOResult& res = serialDevice.beginRead(buffer, 300);
            selector.complete(res);
            bool available = selector.wait();
            if(available)
            {
                size_t n = serialDevice.endRead(res);
                count += n;
                std::cerr<<count<<std::endl;
                //std::cerr.write(buffer, n);
            }*/

            DWORD waitMask = 0;        
        
            std::cerr<<"Start waiting: "<<waitCommMask<<std::endl;
            bool retVal = ( WaitCommEvent( ((Pt::System::SerialDeviceImpl*)serialDevice.impl())->_handle, &waitMask, NULL )  == TRUE );

            std::cerr<<"ComEvent: "<<waitMask<<std::endl;

            if (ReadFile( ((Pt::System::SerialDeviceImpl*)serialDevice.impl())->_handle, buffer, size, &length, 0 ) == FALSE)
            {
                std::cerr<<"Read error"<<std::endl;
            }
            std::cerr.write(buffer, length) << std::endl;
        }

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
