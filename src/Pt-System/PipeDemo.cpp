#include <Pt/Main.h>
#include <Pt/System/Pipe.h>
#include <Pt/System/Selector.h>
#include <Pt/System/FileDevice.h>
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

    Pt::System::FileDevice file("test.txt", std::ios::in | std::ios::out);
    file.write(out.c_str(), out.size());
    file.close();

    file.open("test.txt", std::ios::in | std::ios::out, Pt::System::IODevice::Asynchronous);

    //Pt::System::Pipe pipe;
    //connect(pipe.input().inputReady, onInput);
    //pipe.output().write(out.c_str(), out.size());
    //pipe.output().sync();

    Pt::System::Selector selector;

    while(true)
    {
        Pt::System::IOResult& res = file.beginRead(buffer, size);
        //Pt::System::IOResult& res = pipe.input().beginRead(buffer, size);
        selector.waitInput(res);

        bool avail = selector.wait(1000);
        //if(avail == false)
        //    break;

        sz = file.endRead(res);

        if ( file.eof() )
            break;
        //sz = pipe.input().endRead(res);
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
