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
    pipe.output().sync();

    Pt::System::Selector selector;
    selector.addDevice( pipe.input(), Pt::System::Selector::WaitInput );

	while(true)
	{
		Pt::System::IOResult& res = pipe.input().beginRead(buffer, size);

		// TODO: Here we need to add the IOResult to the Selector

		bool avail = selector.wait(1);

		sz = pipe.input().endRead(res);
		std::cerr.write( buffer, sz ) << "#";

		if(avail == false)
			break;
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

    std::cerr << std::endl;
    return 0;
}
