#include <Pt/Main.h>
#include <Pt/System/SerialDevice.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Selector.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

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
    std::memset( buffer, 0, 201);

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


class ReaderThread : public Pt::System::Thread
{
    public:
        ReaderThread()
        : _sdev("COM1:", std::ios_base::in)
        {
            _sdev.setBaudRate(Pt::System::SerialDevice::BaudRate1200);
            _sdev.setCharSize(7);
            _sdev.setStopBits(Pt::System::SerialDevice::OneStopBit);
            _sdev.setParity(Pt::System::SerialDevice::ParityNone);
            _sdev.setFlowControl(Pt::System::SerialDevice::FlowControlHard);
            _sdev.setTimeout(100);
        }

    protected:
        void run()
        {


            char buffer[200];
            while(true)
            {
                size_t readBytes = _sdev.read(buffer, 200);
                if(readBytes > 0)
                    std::cerr.write(buffer, readBytes) << std::endl;
            }
        }

    private:
        Pt::System::SerialDevice _sdev;
};


int main( int argc, char* argv[] )
{
    //ReaderThread thr;
    //thr.start();
    //thr.wait();
    //return 0;

    try
    {
        const size_t size = 1024;
        char buffer[size];

        //std::string port = "/dev/ttyS0";
        std::string port = "COM1:";
        std::cerr << "Opening " << port << std::endl;

        Pt::System::SerialDevice serialDevice(port, std::ios_base::out, Pt::System::IODevice::Async);
        serialDevice.setBaudRate(Pt::System::SerialDevice::BaudRate4800);
        serialDevice.setCharSize(8);
        serialDevice.setStopBits(Pt::System::SerialDevice::OneStopBit);
        serialDevice.setParity(Pt::System::SerialDevice::ParityNone);
        serialDevice.setFlowControl(Pt::System::SerialDevice::FlowControlHard);
        serialDevice.setTimeout(100);

        Pt::System::Selector selector;

        size_t count = 0;
        std::string msg("Dies ist ein Test\r\n");

        while(true)
        {
            Pt::System::IOResult& res = serialDevice.beginWrite(buffer, size);
            selector.add(res);
            bool available = selector.wait();
            if(available)
            {
                size_t n = serialDevice.endWrite(res);
                std::cerr << "Bytes written: " << n << std::endl;
                count += n;
                //std::cerr<<count<<std::endl;
                //std::cerr.write(buffer, n);
            }
        }

        serialDevice.close();

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
