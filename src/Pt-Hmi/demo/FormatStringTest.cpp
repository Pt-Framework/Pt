#include <iostream>


#include <Pt/fmt/format.h>

#include "FormatString.h"


int main(int argc, char* args[])
{
    std::cerr << fmt::format(
                     "{} {:d} {:.1f} {:p} {}\n",
                     "Test", 123, 456.789, (void*) 1234567890, true
                 );

    std::cerr << Pt::format_string(
                     "{} {:d} {:.1f} {:p} {}\n",
                     "Test", 123, 456.789, (void*) 1234567890, true
                 ).narrow();

    return 0;
}


