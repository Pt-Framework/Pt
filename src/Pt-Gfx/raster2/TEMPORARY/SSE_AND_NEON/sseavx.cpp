// Check for SSE and AVX support
//
// Based on: https://gist.github.com/hi2p-perim/7855506
//           https://software.intel.com/en-us/blogs/2011/04/14/is-avx-enabled
//           https://software.intel.com/en-us/articles/how-to-detect-new-instruction-support-in-the-4th-generation-intel-core-processor-family
//           https://en.wikipedia.org/wiki/CPUID

// g++ -O2 sseavx.cpp -o sseavx && ./sseavx && rm -f sseavx


#include <stdint.h>
#include <iostream>

#ifdef _MSC_VER
#include <intrin.h>
#endif


static inline void cpuidex(uint32_t* abcd_, uint32_t eax_, uint32_t ecx_)
{
#if defined (_MSC_VER) || defined (__INTEL_COMPILER)

    __cpuidex(abcd_, eax_, ecx_);

#elif defined(__GNUC__) || defined(__clang__)

    uint32_t ebx_, edx_;

#if defined(__i386__) && defined (__PIC__)
    // In case of PIC under 32-bit, EBX cannot be clobbered
    __asm__ __volatile__ (
        "movl  %%ebx, %%edi \n\t"
        "cpuid              \n\t"
        "xchgl %%ebx, %%edi"
        : "+a"(eax_),
          "=D"(ebx_),
          "+c"(ecx_),
          "=d"(edx_)
    );
#else
    __asm__ __volatile__ (
        "cpuid"
        : "+b"(ebx_),
          "+a"(eax_),
          "+c"(ecx_),
          "=d"(edx_)
    );
#endif

    abcd_[0] = eax_;
    abcd_[1] = ebx_;
    abcd_[2] = ecx_;
    abcd_[3] = edx_;

#else

    __asm {
        mov eax       , eax_
        mov ecx       , ecx_
        cpuid
        mov esi       , abcd_
        mov [esi     ], eax
        mov [esi +  4], ebx
        mov [esi +  8], ecx
        mov [esi + 12], edx
    }

#endif
}

static inline uint64_t xgetbv(uint32_t idx_)
{
#if (defined (_MSC_FULL_VER) && _MSC_FULL_VER >= 160040000) || (defined (__INTEL_COMPILER) && __INTEL_COMPILER >= 1200)

    return _xgetbv(idx_);

#elif defined(__GNUC__)

    uint32_t eax_, edx_;

    __asm__ __volatile__ (
        "xgetbv;"
        : "=a"(eax_),
          "=d"(edx_)
        :  "c"(idx_)
    );

    return ((uint64_t) edx_ << 32) | eax_;

#else

   uint32_t eax_, edx_;

    __asm {
        mov ecx , idx_
        _emit 0x0F
        _emit 0x01
        _emit 0xD0
        mov eax_, eax
        mov edx_, edx
    }

    return ((uint64_t) edx_ << 32) | eax_;

#endif
}


int main()
{
    // Buffer for CPUID
    uint32_t cpuinfo[4];

    // MMX and SSE1/2/3/4.1/4.2
    cpuidex(cpuinfo, 1, 0);
    const bool   mmxSupportted = (                    ( cpuinfo[3] & (1 << 23) ) );
    const bool fxsaveSupported = (                    ( cpuinfo[3] & (1 << 24) ) );
    const bool  sse1Supportted = ( fxsaveSupported && ( cpuinfo[3] & (1 << 25) ) );
    const bool  sse2Supportted = ( fxsaveSupported && ( cpuinfo[3] & (1 << 26) ) );
    const bool  sse3Supportted = ( fxsaveSupported && ( cpuinfo[2] & (1 <<  0) ) );
    const bool ssse3Supportted = ( fxsaveSupported && ( cpuinfo[2] & (1 <<  9) ) );
    const bool sse41Supportted = ( fxsaveSupported && ( cpuinfo[2] & (1 << 19) ) );
    const bool sse42Supportted = ( fxsaveSupported && ( cpuinfo[2] & (1 << 20) ) );

    // SSE4A and XOP
    cpuidex(cpuinfo, 0x80000000, 0);
    const uint32_t numExtendedIDs = cpuinfo[0];
    if(numExtendedIDs >= 0x80000001) cpuidex(cpuinfo, 0x80000001, 0);

    const bool sse4aSupportted = ( numExtendedIDs >= 0x80000001 ) && ( cpuinfo[2] & (1 <<  6) );
    const bool   xopSupportted = ( numExtendedIDs >= 0x80000001 ) && ( cpuinfo[2] & (1 << 11) );

    // AVX
    cpuidex(cpuinfo, 1, 0);
    const bool     oxsaveSupported = ( cpuinfo[2] & (1 << 27) );
    const uint64_t xfeatureEnMask  = oxsaveSupported ? xgetbv(0 /* _XCR_XFEATURE_ENABLED_MASK */) : 0;
    const bool     ymmEnabledByOS  = ( ( xfeatureEnMask & 0x06 ) == 0x06 );
    const bool     avx1Supportted  = ( ymmEnabledByOS && ( cpuinfo[2] & (1 << 28) ) );

    // AVX2 and BMI1/2
    cpuidex(cpuinfo, 7, 0);
    const bool avx2Supportted = avx1Supportted && ( cpuinfo[1] & (1 << 5) );
    const bool bmi1Supportted = avx1Supportted && ( cpuinfo[1] & (1 << 3) );
    const bool bmi2Supportted = avx1Supportted && ( cpuinfo[1] & (1 << 8) );

    // FMA3/4
    cpuidex(cpuinfo, 1, 0);
    const bool fma3Supportted = avx1Supportted && ( cpuinfo[2] & (1 << 12) );
    const bool fma4Supportted = avx1Supportted && ( cpuinfo[2] & (1 << 16) );

    // AVX512
    cpuidex(cpuinfo, 7, 0);
    const bool       zmmEnabledByOS = ( ( xfeatureEnMask & 0x26 ) == 0x26 );
    const bool    avx512FSupportted = zmmEnabledByOS && ( cpuinfo[1] & (1 << 16) ); // AVX-512 foundation
    const bool   avx512CDSupportted = zmmEnabledByOS && ( cpuinfo[1] & (1 << 28) ); // AVX-512 conflict detection      instructions
    const bool   avx512VLSupportted = zmmEnabledByOS && ( cpuinfo[1] & (1 << 31) ); // AVX-512 vector length           extensions
    const bool   avx512BWSupportted = zmmEnabledByOS && ( cpuinfo[1] & (1 << 30) ); // AVX-512 byte       and word     instructions
    const bool   avx512DQSupportted = zmmEnabledByOS && ( cpuinfo[1] & (1 << 17) ); // AVX-512 doubleword and quadword instructions
    const bool avx512IFMASupportted = zmmEnabledByOS && ( cpuinfo[1] & (1 << 21) ); // AVX-512 integer fused mul-add   instructions
    const bool avx512VBMISupportted = zmmEnabledByOS && ( cpuinfo[2] & (1 <<  1) ); // AVX-512 vector bit manipulation instructions
    const bool   avx512ERSupportted = zmmEnabledByOS && ( cpuinfo[1] & (1 << 27) ); // AVX-512 exponential-reciprocal  instructions
    const bool   avx512PFSupportted = zmmEnabledByOS && ( cpuinfo[1] & (1 << 26) ); // AVX-512 prefetch                instructions

    // Outputs the result
    std::cout << "MMX        : " <<         mmxSupportted << std::endl;
    std::cout << "SSE1       : " <<        sse1Supportted << std::endl;
    std::cout << "SSE2       : " <<        sse2Supportted << std::endl;
    std::cout << "SSE3       : " <<        sse3Supportted << std::endl;
    std::cout << "SSE4.1     : " <<       sse41Supportted << std::endl;
    std::cout << "SSE4.2     : " <<       sse42Supportted << std::endl;
    std::cout << "SSE4A      : " <<       sse4aSupportted << std::endl;
    std::cout << "XOP        : " <<         xopSupportted << std::endl;
    std::cout << "AVX1       : " <<        avx1Supportted << std::endl;
    std::cout << "AVX2       : " <<        avx2Supportted << std::endl;
    std::cout << "FMA3       : " <<        fma3Supportted << std::endl;
    std::cout << "FMA4       : " <<        fma4Supportted << std::endl;
    std::cout << "BMI1       : " <<        bmi1Supportted << std::endl;
    std::cout << "BMI2       : " <<        bmi2Supportted << std::endl;
    std::cout << "AVX512F    : " <<     avx512FSupportted << std::endl;
    std::cout << "AVX512CD   : " <<    avx512CDSupportted << std::endl;
    std::cout << "AVX512VL   : " <<    avx512VLSupportted << std::endl;
    std::cout << "AVX512BW   : " <<    avx512BWSupportted << std::endl;
    std::cout << "AVX512DQ   : " <<    avx512DQSupportted << std::endl;
    std::cout << "AVX512IFMA : " <<  avx512IFMASupportted << std::endl;
    std::cout << "AVX512BMI  : " <<  avx512VBMISupportted << std::endl;
    std::cout << "AVX512ER   : " <<    avx512ERSupportted << std::endl;
    std::cout << "AVX512PF   : " <<    avx512PFSupportted << std::endl;

    return 0;
}
