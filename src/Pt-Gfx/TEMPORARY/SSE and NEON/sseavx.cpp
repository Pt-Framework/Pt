// Check for SSE and AVX support
// Based on https://gist.github.com/hi2p-perim/7855506
//          https://software.intel.com/en-us/blogs/2011/04/14/is-avx-enabled
//          https://software.intel.com/en-us/articles/how-to-detect-new-instruction-support-in-the-4th-generation-intel-core-processor-family
//          https://en.wikipedia.org/wiki/CPUID

// g++ -O2 sseavx.cpp -o sseavx && ./sseavx && rm -f sseavx


#include <stdint.h>
#include <iostream>

// Minimum VS2010 SP1 compiler is required
#ifdef _MSC_VER
#include <intrin.h>
#endif


#ifdef __GNUC__

void __cpuidex(uint32_t* abcd, uint32_t eax, uint32_t ecx)
{
    uint32_t ebx, edx;

#if defined(__i386__) && defined (__PIC__)
    // In case of PIC under 32-bit, EBX cannot be clobbered
    __asm__ __volatile__ (
        "movl  %%ebx, %%edi \n\t"
        "cpuid              \n\t"
        "xchgl %%ebx, %%edi"
        : "+a"(eax),
          "=D"(ebx),
          "+c"(ecx),
          "=d"(edx)
    );
#else
    __asm__ __volatile__ (
        "cpuid"
        : "+b"(ebx),
          "+a"(eax),
          "+c"(ecx),
          "=d"(edx)
    );
#endif

    abcd[0] = eax;
    abcd[1] = ebx;
    abcd[2] = ecx;
    abcd[3] = edx;
}

uint64_t _xgetbv(uint32_t index)
{
    uint32_t eax, edx;

    __asm__ __volatile__ (
        "xgetbv;"
        : "=a"(eax),
          "=d"(edx)
        :  "c"(index)
    );

    return ((uint64_t) edx << 32) | eax;
}

#endif


int main()
{
    uint32_t cpuinfo[4];

    __cpuidex(cpuinfo, 1, 0);
    const bool  sse1Supportted = ( cpuinfo[3] & (1 << 25) );
    const bool  sse2Supportted = ( cpuinfo[3] & (1 << 26) );
    const bool  sse3Supportted = ( cpuinfo[2] & (1 <<  0) );
    const bool ssse3Supportted = ( cpuinfo[2] & (1 <<  9) );
    const bool sse41Supportted = ( cpuinfo[2] & (1 << 19) );
    const bool sse42Supportted = ( cpuinfo[2] & (1 << 20) );

    __cpuidex(cpuinfo, 0x80000000, 0);
    const uint32_t numExtendedIDs = cpuinfo[0];
    if(numExtendedIDs >= 0x80000001) __cpuidex(cpuinfo, 0x80000001, 0);

    const bool sse4aSupportted = ( numExtendedIDs >= 0x80000001 ) && ( cpuinfo[2] & (1 <<  6) );
    const bool   xopSupportted = ( numExtendedIDs >= 0x80000001 ) && ( cpuinfo[2] & (1 << 11) );

    __cpuidex(cpuinfo, 1, 0);
    const bool avxBitInCPU    = ( cpuinfo[2] & (1 << 28) );
    const bool xsaveBitInCPU  = ( cpuinfo[2] & (1 << 27) );
    const bool avx1Supportted = ( avxBitInCPU && xsaveBitInCPU && ( ( _xgetbv(0) & 0x06 ) == 0x06 ) );

    __cpuidex(cpuinfo, 7, 0);
    const bool avx2Supportted = avx1Supportted && ( cpuinfo[1] & (1 << 5) );

    __cpuidex(cpuinfo, 1, 0);
    const bool fma3Supportted = avx1Supportted && ( cpuinfo[2] & (1 << 12) );

    std::cout << "SSE1   : " <<  sse1Supportted << std::endl;
    std::cout << "SSE2   : " <<  sse2Supportted << std::endl;
    std::cout << "SSE3   : " <<  sse3Supportted << std::endl;
    std::cout << "SSE4.1 : " << sse41Supportted << std::endl;
    std::cout << "SSE4.2 : " << sse42Supportted << std::endl;
    std::cout << "SSE4a  : " << sse4aSupportted << std::endl;
    std::cout << "XOP    : " <<   xopSupportted << std::endl;
    std::cout << "AVX1   : " <<  avx1Supportted << std::endl;
    std::cout << "AVX2   : " <<  avx2Supportted << std::endl;
    std::cout << "FMA3   : " <<  fma3Supportted << std::endl;

    return 0;
}
