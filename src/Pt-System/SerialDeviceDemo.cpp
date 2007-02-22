#include <Pt/Main.h>
#include <Pt/System/SerialDevice.h>
#include <Pt/System/Thread.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>

void readMouseData()
{
    //std::string port("/dev/ttyS0");
    std::string port("COM1:");
    std::cerr << "Opening " <<port << std::endl;
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

void readNMEA()
{
    std::ofstream fs("serial.txt");
    std::cerr.rdbuf( fs.rdbuf() );
    Pt::System::Thread::sleep( 10000 );

    try
    {
        std::stringstream ss;
        ss<<"COM5:";

        std::cerr << "Opening " << ss.str() << std::endl;
        Pt::System::SerialDevice serdev( ss.str().c_str(),  std::ios_base::out );

        std::cerr << "Setting baud rate " << std::endl;
        serdev.setBaudRate(Pt::System::SerialDevice::BaudRate4800);
        serdev.setCharSize(7);
        serdev.setStopBits(Pt::System::SerialDevice::OneStopBit);

        std::cerr << "Setting flow control" << std::endl;
        serdev.setFlowControl(Pt::System::SerialDevice::FlowControlHard); 
        Pt::System::Thread::sleep( 300 );
        serdev.setFlowControl(Pt::System::SerialDevice::FlowControlSoft);

        Pt::System::Thread::sleep( 300 );

        char buffer[200];
        memset( buffer, 0, 200);
        size_t size = serdev.read( buffer, 200);

        std::cerr<<"Byte readed: "<<size<<std::endl;

        std::cerr<<"Data: "<<buffer;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

class Reader : public Pt::System::Thread
{
    public:
        Reader( Pt::System::SerialDevice& serDev_ )
        : serDev( serDev_ )
        { }
        
        ~Reader()
        {}
        
        void run()
        {
            bool retVal = serDev.wait( Pt::System::SerialDevice::WaitInput, Pt::System::SerialDevice::WaitTimeInfinite );
        }
        
        Pt::System::SerialDevice& serDev;
};

void waitEventDemo()
{
    try
    {
        Pt::System::SerialDevice serDev("COM1:", std::ios_base::in | std::ios_base::out);
        Reader reader(serDev);    
        reader.start();
        
        Pt::System::Thread::sleep( 5000 );
        serDev.close();             
        reader.wait();
    }
    catch( const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;    
    }
}

int main( int argc, char* argv[] )
{    
    waitEventDemo();
    return 0;
}
