/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <Pt/Main.h>
#include <Pt/System/Pipe.h>
#include <Pt/System/Selector.h>
#include <iostream>

//#include <Pt/System/FileDevice.h>

void onInput(Pt::System::IODevice& r)
{
    std::cerr << "Input: ";
}

void onOutput(Pt::System::IODevice& r)
{
    std::cerr << "Output done.\n";
}


void demo2()
{
    Pt::System::Selector selector;

    Pt::System::Pipe pipe(Pt::System::IODevice::Async);
    Pt::System::Pipe pipe2(Pt::System::IODevice::Async);

    pipe2.in().write("ABCDE", 5);

    selector.add( pipe.out() );
    selector.add( pipe2.out() );
    selector.wait(100);
   
    char buffer[10];
    pipe2.out().beginRead(buffer, 1);
   
    char buf[10];
    pipe.out().beginRead(buf, 5);

    std::cerr << "WAITING 5000 ms" << std::endl;
    pipe.in().write("Hello", 5);
    selector.wait(5000);

    unsigned n = pipe2.out().endRead();
    std::cerr << "READ: " << n << std::endl;
    std::cerr.write(buffer, 1) << std::endl;
    
    n = pipe.out().endRead();
    std::cerr << "READ: " << n << std::endl;
    std::cerr.write(buf, 5) << std::endl;
}

void demo1()
{
    std::string out("Hello World, where do you want to GOTO day!");
    const int size = 50;
    char buffer[size];
    std::size_t bytes = 0;

    Pt::System::Pipe pipe(Pt::System::IODevice::Async);
    connect(pipe.out().inputReady, onInput);
    connect(pipe.in().outputReady, onOutput);

    Pt::System::Selector selector;
    
    pipe.out().beginRead(buffer, 10);

    selector.add( pipe.out() );
    bool avail = selector.wait(500);

    pipe.in().beginWrite( out.c_str() + bytes, out.size() - bytes ); 

    avail = pipe.out().wait(500);
    unsigned x = pipe.out().endRead();
    //selector.add( pipe.in() );
}

int main( int argc, char* argv[] )
{
    try 
    {
		demo2();
		return 0;
	
        std::string out("Hello World, where do you want to GOTO day!");
        const int size = 50;
        char buffer[size];
        std::size_t bytes = 0;
    
        Pt::System::Pipe pipe(Pt::System::IODevice::Async);
        connect(pipe.out().inputReady, onInput);
        connect(pipe.in().outputReady, onOutput);
    
        Pt::System::Selector selector;
        selector.add( pipe.out() );
        selector.add( pipe.in() );
        
        while(true)
        {
            pipe.in().beginWrite( out.c_str() + bytes, out.size() - bytes ); 
            bool active = selector.wait(500);
            if(active == false)
            {
                std::cerr << "main: return -1" << std::endl;
                return -1;
            }
    
            bytes += pipe.in().endWrite();
            if( bytes == out.size() )
                break;
        }
        
        selector.remove( pipe.in() );
        
        bytes = 0;
        while(true)
        {
            pipe.out().beginRead(buffer, size);
        
            bool active = selector.wait(500);
            if(active == false)
            {
                std::cerr << "main: return -1" << std::endl;
                return -1;
            }
    
            bytes += pipe.out().endRead();
            std::cerr.write( buffer, bytes ) << "\n";
            
            if( bytes == out.size() )
                break;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    
    std::cerr << "main: return 0" << std::endl;
    return 0;
}
