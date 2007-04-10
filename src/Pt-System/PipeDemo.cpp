#include <Pt/Main.h>
#include <Pt/System/Pipe.h>
#include <Pt/System/Selector.h>
#include <iostream>


int main( int argc, char* argv[] )
{
    Pt::System::Pipe pipe;

    const char* out = "Hello World!";
    pipe.output().write(out, 12);

    Pt::System::Selector selector;
    selector.addDevice( pipe.input(), Pt::System::Selector::WaitInput );
    bool ret = selector.wait();

    std::cerr << "Data: " << std::boolalpha << ret << std::endl;
    char buffer[20];
    size_t sz = pipe.input().read(buffer, 20);
    std::cerr.write(buffer, sz);
    std::cerr << std::endl;

    return 0;
}
