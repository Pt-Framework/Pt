#include <Pt/Main.h>
#include <Pt/System/Pipe.h>
#include <Pt/System/Selector.h>
#include <iostream>

int main( int argc, char* argv[] )
{
    Pt::System::Pipe pipe;

    std::string out("Hello World, where do you want to GOTO day!");
    pipe.output().write(out.c_str(), out.size());

    Pt::System::Selector selector;
    selector.addDevice( pipe.input(), Pt::System::Selector::WaitInput );
    
    const int size = 10;
    char buffer[size];
    size_t sz;

    bool ret = selector.wait();    
    std::cerr << "Data: " << std::boolalpha << ret << std::endl;
    do
    {
        memset(&buffer, 0, size);
        sz = pipe.input().read(buffer, size);
        std::cerr.write(buffer, sz);        
    }
    while( selector.wait(100) );    
    std::cerr << std::endl;

    return 0;
}
