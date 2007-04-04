#include <Pt/Main.h>
#include <Pt/System/Pipe.h>
#include <Pt/System/IOMonitor.h>
#include <iostream>


int main( int argc, char* argv[] )
{
    Pt::System::Pipe pipe;

    const char* out = "Hello World!";
    pipe.output().write(out, 12);

    Pt::System::IOMonitor monitor;
    monitor.addDevice( pipe.input(), Pt::System::IODevice::WaitInput );
    bool ret = monitor.wait();

    std::cerr << "Data: " << std::boolalpha << ret << std::endl;
    char buffer[20];
    size_t sz = pipe.input().read(buffer, 20);
    std::cerr.write(buffer, sz);

    monitor.removeDevice( pipe.input() );
    std::cerr << std::endl;
    return 0;
}
