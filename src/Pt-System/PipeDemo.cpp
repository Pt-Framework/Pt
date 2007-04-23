#include <Pt/Main.h>
#include <Pt/System/Pipe.h>
#include <Pt/System/Selector.h>
#include <iostream>

int main( int argc, char* argv[] )
{
    const int size = 8;
    char buffer[size];
    size_t sz;

    Pt::System::Pipe pipe;

    std::string out("Hello World, where do you want to GOTO day!");
    pipe.output().write(out.c_str(), out.size());

    Pt::System::Selector selector;
    Pt::System::IOResult res = pipe.input().beginRead(buffer, size);

    selector.addDevice( pipe.input(), Pt::System::Selector::WaitInput );

    bool ret = selector.wait();
    std::cerr << "Data: " << std::boolalpha << ret << std::endl;

    do
    {
        sz = pipe.input().read(buffer, size);
        std::cerr.write(buffer, sz);
    }
    while( selector.wait(100) );

    std::cerr << std::endl;
    return 0;
}
