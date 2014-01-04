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
#include <fstream>
#include <iostream>

int main( int argc, char** argv )
{
    if( argc != 4)
    {
        std::cerr << "Usage: MakeFontHeader input.ttf output.h FontName" << std::endl;
        return -1;
    }
    
    std::ifstream input(argv[1], std::ios::binary);
    std::ofstream output(argv[2]);
    
    output << "#ifndef Pt_Gfx_" << argv[3] << "_h\n";
    output << "#define Pt_Gfx_" << argv[3] << "_h\n\n";

    output << "namespace Pt{\n\n";
    output << "namespace Gfx{\n\n";
    
    output << std::endl;
    output << "static const unsigned char " << argv[3] << "[] = {" << std::endl;
    
    output<<std::hex;
    char ch;
    int count = 0;
    while( input.get(ch) )
    {
        output << "0x"<<(int) (unsigned char) ch;
        
        if( input.peek() != std::char_traits<char>::eof() )
            output << ", ";  
            
        if( ++count % 10 == 0 )
            output<<std::endl;                     
    }

    output << "\n};\n";
    
    output << "\nstatic size_t " << argv[3] << "Size = " << std::dec << count << ";\n\n";
    output << "}\n\n";
    output << "}\n\n";
        
    output<< "#endif "<<std::endl;
    
    return 0;
         
}
