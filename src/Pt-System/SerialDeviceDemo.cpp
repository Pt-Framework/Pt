#include <Pt/Main.h>
#include <Pt/System/SerialDevice.h>
#include <iostream>

int main( int argc, char* argv[] )
{
    try
    {
        const char* buffer = "Hallo World";
        const size_t size  = strlen( buffer ) + 1;
        
        Pt::System::SerialDevice serdev( "COM1",  std::ios_base::out );
        
        serdev.write( buffer, size );
                
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
