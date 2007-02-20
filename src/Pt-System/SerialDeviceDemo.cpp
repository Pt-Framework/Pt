#include <Pt/Main.h>
#include <Pt/System/SerialDevice.h>
#include <Pt/System/Thread.h>
#include <iostream>
#include <fstream>
#include <sstream>


void readMouseData()
{
    Pt::System::SerialDevice serdev( "COM1",  std::ios_base::out );        
    
    serdev.setBaudRate(Pt::System::SerialDevice::BaudRate1200);
    serdev.setCharSize(7);
    serdev.setStopBits(Pt::System::SerialDevice::OneStopBit);
    serdev.setFlowControl(Pt::System::SerialDevice::FlowControlHard); 
    Pt::System::Thread::sleep( 300 );               
    serdev.setFlowControl(Pt::System::SerialDevice::FlowControlSoft);
    
    Pt::System::Thread::sleep( 300 );   
    
    char buffer[200];
    char byte;
    memset( buffer, 0, 200);
    size_t size = serdev.read( buffer, 200);

   //read pnp info
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
    std::cerr<<pnpString<<std::endl;
}

int main( int argc, char* argv[] )
{    
    std::ofstream fs("serial.txt");
    std::cerr.rdbuf( fs.rdbuf() );
    Pt::System::Thread::sleep( 10000 );    
    
        try
        {
            std::stringstream ss;
            ss<<"COM5:";
            
            Pt::System::SerialDevice serdev( ss.str().c_str(),  std::ios_base::out );        
            
            serdev.setBaudRate(Pt::System::SerialDevice::BaudRate4800);
            serdev.setCharSize(7);
            serdev.setStopBits(Pt::System::SerialDevice::OneStopBit);

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
            std::cerr << "Exception: Port :"<< std::endl;
        }

    return 0;
}
