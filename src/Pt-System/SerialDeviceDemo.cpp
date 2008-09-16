#include <Pt/Main.h>
#include <Pt/System/SerialDevice.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Selector.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

void readMousePnp(const std::string& port)
{
    Pt::System::SerialDevice serdev( port, Pt::System::SerialDevice::Read );
    serdev.setBaudRate(Pt::System::SerialDevice::BaudRate1200);
    serdev.setCharSize(7);
    serdev.setStopBits(Pt::System::SerialDevice::OneStopBit);
    serdev.setParity(Pt::System::SerialDevice::ParityNone);
    serdev.setFlowControl(Pt::System::SerialDevice::FlowControlHard);
    Pt::System::Thread::sleep( 300 );

    serdev.setFlowControl(Pt::System::SerialDevice::FlowControlSoft);
    Pt::System::Thread::sleep( 300 );

    char buffer[201];
    char byte;
    std::memset( buffer, 0, 201);

    std::cerr << "Reading PNP data..." << std::endl;
    size_t size = serdev.read( buffer, 1);
    std::cerr << "RAW PNP ID: " << (int) buffer[0] << std::endl;

    if( (int)buffer[0] == 0 )
        return;

    size = serdev.read( buffer, 200);
    std::cerr << "RAW PNP DATA: " << buffer << " (" << size << " bytes)" << std::endl;

    std::cerr << "Parsing PnP data..." << std::endl;
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

    std::cerr << "PNP COOKED DATA: " << pnpString << std::endl << std::endl;
}


const size_t size = 1024;
char buffer[size];
        
        
void onInput(Pt::System::IODevice& dev)
{
    size_t n = dev.endRead();
    std::cerr.write(buffer, n);
    dev.beginRead(buffer, size);
}


int main( int argc, char* argv[] )
{
    if(argc < 2)
    {
        std::cerr << "ERROR: Need a port name." << std::endl;
        return 1;
    }

    try
    {
        std::string port = argv[1]; // COM1: or /dev/ttyS0
        std::cerr << "'=> Opening " << port << std::endl;
        
        readMousePnp(port);

        Pt::System::SerialDevice serialDevice(port, Pt::System::SerialDevice::Read);
        serialDevice.setBaudRate(Pt::System::SerialDevice::BaudRate4800);
        serialDevice.setCharSize(8);
        serialDevice.setStopBits(Pt::System::SerialDevice::OneStopBit);
        serialDevice.setParity(Pt::System::SerialDevice::ParityNone);
        serialDevice.setFlowControl(Pt::System::SerialDevice::FlowControlHard);
        serialDevice.setTimeout(100);
        connect(serialDevice.inputReady, onInput);
        
        Pt::System::Selector selector;
        selector.add(serialDevice);

        serialDevice.beginRead(buffer, size);
        while(true)
        {
            selector.wait();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
