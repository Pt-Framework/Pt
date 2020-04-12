#include <iostream>


#include <Pt/fmt/format.h>

#include "FormatString.h"


int main(int argc, char* args[])
{
    std::cerr << fmt::format(
                     "{} {:d} {:.1f}\n",
                     "Test", 123, 456.789
                 );
    
    std::cerr << Pt::format_string(
                     "{} {:d} {:.1f}\n",
                     "Test", 123, 456.789
                 );

    return 0;
}


