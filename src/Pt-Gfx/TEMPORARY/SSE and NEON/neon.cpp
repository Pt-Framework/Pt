// Check for NEON support
//
// Based on: How to check the existence of NEON on arm?
//           http://stackoverflow.com/questions/26701262/how-to-check-the-existence-of-neon-on-arm
//           Answer by Peter M, 2014 (permalink: http://stackoverflow.com/a/26920837)

// g++ -O2 neon.cpp -o neon && ./neon && rm -f neon


#include <iostream>

#include <sys/auxv.h>
#include <asm/hwcap.h>


int main()
{
    const bool neonSupportted = getauxval(AT_HWCAP) & HWCAP_NEON;

    std::cout << "NEON : " <<  neonSupportted << std::endl;

    return 0;
}
