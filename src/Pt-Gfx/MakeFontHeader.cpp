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
