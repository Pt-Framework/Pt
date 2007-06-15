#include <Pt/Main.h>
#include <Pt/System/Pipe.h>
#include <Pt/System/Selector.h>
#include <Pt/System/FileDevice.h>
#include <iostream>
#include <fstream>

void onInput(Pt::System::IOResult& r)
{
    std::cerr << "Input: ";
}

int main( int argc, char* argv[] )
{
    std::string out("Hello World, where do you want to GOTO day!");
    const int size = 8;
    char buffer[size];
    size_t sz;

  /*  Pt::System::FileDevice file("test.txt", std::ios::in | std::ios::out);
    file.write(out.c_str(), out.size());
    file.close();

    file.open("test.txt", std::ios::in|std::ios::out, Pt::System::IODevice::Asynchronous);
	file.seek(0, Pt::System::IODevice::SeekBegin);*/

    Pt::System::Pipe pipe(Pt::System::IODevice::Async);
    connect(pipe.input().inputReady, onInput);
    pipe.output().write(out.c_str(), out.size());    

    Pt::System::Selector selector;

    while(true)
    {
       /* Pt::System::IOResult& res = file.beginRead(buffer, size);
        if ( file.eof() )
           break;*/

        Pt::System::IOResult& res = pipe.input().beginRead(buffer, size);
        selector.complete(res);

        bool avail = selector.wait(500);
        if(avail == false)
            break;

        /*sz = file.endRead(res);
        if ( file.eof() )
            break;*/

        sz = pipe.input().endRead(res);
        std::cerr.write( buffer, sz ) << "\n";
    }

    std::cerr << "\nSync read with async pipe:\n";
    Pt::System::Pipe pipe2(Pt::System::IODevice::Async);
    pipe2.output().write(out.c_str(), out.size());

    size_t readBytes = pipe2.input().read(buffer, size);
    std::cerr.write(buffer, readBytes);

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
