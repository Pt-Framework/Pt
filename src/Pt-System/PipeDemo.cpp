#include <Pt/Main.h>
#include <Pt/System/Pipe.h>
#include <Pt/System/Selector.h>
#include <iostream>

void onInput()
{
    std::cerr << "Input: ";
}

int main( int argc, char* argv[] )
{
    std::string out("Hello World, where do you want to GOTO day!");
    const int size = 8;
    char buffer[size];
    size_t sz;

    Pt::System::Pipe pipe;
    connect(pipe.input().inputReady, onInput);
    pipe.output().write(out.c_str(), out.size());
    pipe.output().sync();

    Pt::System::Selector selector;

    while(true)
    {
        Pt::System::IOResult& res = pipe.input().beginRead(buffer, size);
        selector.waitInput(res);

        bool avail = selector.wait(100);
        if(avail == false)
            break;

        sz = pipe.input().endRead(res);
        std::cerr.write( buffer, sz ) << "\n";
    }

    std::cerr << std::endl;
    return 0;
}







/*bool ret = selector.wait(1000);
sz = pipe.input().endRead(res);
std::cerr.write( buffer, sz ) << "|";

res = pipe.input().beginRead(buffer, size);
do
{
    sz = pipe.input().endRead(res);
    std::cerr.write( buffer, sz ) << "|";

    res = pipe.input().beginRead(buffer, size);
}
while( selector.wait(100) );*/
