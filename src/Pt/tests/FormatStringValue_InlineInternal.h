/* Copyright (C) 2020-2020 Aloysius Indrayanto
 *
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

#ifndef PT_FORMAT_STRING_VALUE_INLINE_INTERNAL_H
#define PT_FORMAT_STRING_VALUE_INLINE_INTERNAL_H


namespace Pt {


//
// Utility macros
//
#define TYPE_IS_N(T)    ( !T                                                                                        )

#define TYPE_IS_S(T)    ( !T || ( T &&   (T == Rule::STRING              )                                        ) )
#define TYPE_IS_C(T)    ( !T || ( T &&   (T == Rule::CHARACTER           )                                        ) )

#define TYPE_IS_B(T)    (       ( T && ( (T == Rule::BINARY_LOWER        ) || (T == Rule::BINARY_UPPER        ) ) ) )
#define TYPE_IS_D(T)    ( !T || ( T &&   (T == Rule::DECIMAL             )                                        ) )
#define TYPE_IS_O(T)    (       ( T &&   (T == Rule::OCTAL               )                                        ) )
#define TYPE_IS_X(T)    (       ( T && ( (T == Rule::HEXADECIMAL_LOWER   ) || (T == Rule::HEXADECIMAL_UPPER   ) ) ) )

#define TYPE_IS_A(T)    (       ( T && ( (T == Rule::HEXADECIMAL_FP_LOWER) || (T == Rule::HEXADECIMAL_FP_UPPER) ) ) )
#define TYPE_IS_E(T)    (       ( T && ( (T == Rule::SCIENTIFIC_FP_LOWER ) || (T == Rule::SCIENTIFIC_FP_UPPER ) ) ) )
#define TYPE_IS_F(T)    (       ( T && ( (T == Rule::DECIMAL_FP_LOWER    ) || (T == Rule::DECIMAL_FP_UPPER    ) ) ) )
#define TYPE_IS_G(T)    (       ( T && ( (T == Rule::SHORTEST_FP_LOWER   ) || (T == Rule::SHORTEST_FP_UPPER   ) ) ) )

#define TYPE_IS_P(T)    ( !T || ( T &&   (T == Rule::POINTER             )                                        ) )

#define TYPE_IS_BDOX(T) ( T && ( (T == Rule::BINARY_LOWER     ) || (T == Rule::BINARY_UPPER     ) || \
                                 (T == Rule::DECIMAL          ) || (T == Rule::OCTAL            ) || \
                                 (T == Rule::HEXADECIMAL_LOWER) || (T == Rule::HEXADECIMAL_UPPER)    \
                               )                                                                     \
                        )

#define TYPE_IS_AEFG(T) ( T && ( (T == Rule::HEXADECIMAL_FP_LOWER) || (T == Rule::HEXADECIMAL_FP_UPPER) || \
                                 (T == Rule::SCIENTIFIC_FP_LOWER ) || (T == Rule::SCIENTIFIC_FP_UPPER ) || \
                                 (T == Rule::DECIMAL_FP_LOWER    ) || (T == Rule::DECIMAL_FP_UPPER    ) || \
                                 (T == Rule::SHORTEST_FP_LOWER   ) || (T == Rule::SHORTEST_FP_UPPER   )    \
                               )                                                                           \
                        )


//
// Utilities for selecting integer from FormatStringValue::_valPOD
//
template <typename ValueT>
struct FormatStringValue::SelectInt {};


template<>
struct FormatStringValue::SelectInt<Pt::int32_t> {
    typedef Pt::int32_t  SignedT;
    typedef Pt::uint32_t UnsignedT;

    static inline Pt::int32_t  selectSigned  (const FormatStringValue* inst) { return inst->_valPOD.i32; }
    static inline Pt::uint32_t selectUnsigned(const FormatStringValue* inst) { return inst->_valPOD.u32; }


};


template<>
struct FormatStringValue::SelectInt<Pt::int64_t> {
    typedef Pt::int64_t  SignedT;
    typedef Pt::uint64_t UnsignedT;

    static inline Pt::int64_t  selectSigned  (const FormatStringValue* inst) { return inst->_valPOD.i64; }
    static inline Pt::uint64_t selectUnsigned(const FormatStringValue* inst) { return inst->_valPOD.u64; }
};


//
// Utilities for formatting (printing) unsigned value
//

// Base class that contains the common functions
struct FormatStringValue::FormatUnsigned_Common {
    template <typename ValueT, unsigned int BASE>
    static inline Pt::uint32_t countNumberOfUnsignedDigits(ValueT val);

    static inline void reverseAndGroupString(Pt::String& dst, const Pt::String& src, Pt::Char thousandsSep, Pt::uint8_t groupingSize);
    static inline void reverseAndGroupString(Pt::String& dst, const Pt::String& src, Pt::Char thousandsSep, const Pt::uint8_t* groupingSizePtr, size_t groupingSizeCount);
};


template <typename ValueT, unsigned int BASE>
inline Pt::uint32_t FormatStringValue::FormatUnsigned_Common::countNumberOfUnsignedDigits(ValueT val)
{
#if 0
    Pt::uint32_t cntDigit = 0;

    while(val) {
        val /= BASE;
        ++cntDigit;
    }

    return cntDigit;
#else
    // It seems to be 1.5x to 45x faster than the above
    return floorf( logl(val) /  logf(BASE) ) + 1;
#endif
}


inline void FormatStringValue::FormatUnsigned_Common::reverseAndGroupString(Pt::String& dst, const Pt::String& src, Pt::Char thousandsSep, Pt::uint8_t groupingSize)
{
    // Get the source length
    const size_t srcLen = src.length();

    // Get the source pointers
    const Pt::Char* srcIt    = &src[srcLen - 1];
    const Pt::Char* srcItEnd = srcIt - srcLen;

    // Process without using thousands separator
    if(!thousandsSep || srcLen <= groupingSize) {
        // Resize the destination buffer
        dst.clear();
        dst.resize(srcLen);
        // Reverse the characters
        Pt::Char* dstIt = &dst[0];
        while(srcIt != srcItEnd) *dstIt++ = *srcIt--;
        // Done
        return;
    }

    // Calculate the number of thousands separator(s)
    const size_t sepCnt = (srcLen + groupingSize - 1) / groupingSize - 1;
    const size_t dstLen = src.length() + sepCnt;

    // Resize the destination buffer
    dst.clear();
    dst.resize(dstLen);

    // Reverse the characters while adding thousands separator(s)
    Pt::Char*    dstIt      = &dst[0];
    Pt::uint32_t digitIndex = groupingSize - (srcLen % groupingSize);
    for(;;) {
        *dstIt++ = *srcIt--;
        if(srcIt == srcItEnd) break;
        if(++digitIndex % groupingSize == 0) *dstIt++ = thousandsSep;
    }
}


inline void FormatStringValue::FormatUnsigned_Common::reverseAndGroupString(Pt::String& dst, const Pt::String& src, Pt::Char thousandsSep, const Pt::uint8_t* groupingSizePtr, size_t groupingSizeCount)
{
    // Get the source length
    const size_t srcLen = src.length();

    // Get the source pointers
    const Pt::Char* srcIt    = &src[0];
    const Pt::Char* srcItEnd = srcIt + srcLen;

    // Implement complex digit grouping (example: \1\2\3 => 18,446,744,073,709,551,61,5)

    // Calculate the number of thousands separator(s)
    size_t idxGz  = 0;
    size_t sepCnt = 0;

    --groupingSizeCount;

    while(srcIt != srcItEnd) {
        // Get the current grouping size
        Pt::uint8_t groupingSize = groupingSizePtr[idxGz];
        if(idxGz < groupingSizeCount) ++idxGz;
        // Process the characters
        for(;;) {
            if(++srcIt == srcItEnd) break;
            if(--groupingSize) continue;
            ++sepCnt;
            break;
        }
    }

    // Resize the destination buffer
    const size_t dstLen = srcLen + sepCnt;

    dst.clear();
    dst.resize(dstLen);

    // Add thousands separator(s)
    Pt::Char* dstIt = &dst[0];
              srcIt = &src[0];
              idxGz = 0;

    while(srcIt != srcItEnd) {
        // Get the current grouping size
        Pt::uint8_t groupingSize = groupingSizePtr[idxGz];
        if(idxGz < groupingSizeCount) ++idxGz;
        // Process the characters
        for(;;) {
            *dstIt++ = *srcIt++;
            if(srcIt == srcItEnd) break;
            if(--groupingSize) continue;
            *dstIt++ = thousandsSep;
            break;
        }
    }

    // Reverse the characters
    for(size_t i = 0; i < (dstLen / 2); ++i) {
        const Pt::Char t = dst[i];
        dst[         i    ] = dst[dstLen - i - 1];
        dst[dstLen - i - 1] = t;
    }
}


// Generic version
template <typename ValueT, int BASE>
struct FormatStringValue::FormatUnsigned : public FormatUnsigned_Common {
    static inline void printReversed(Pt::String& dst, ValueT val, bool uppercase = false);
};


template <typename ValueT, int BASE>
inline void FormatStringValue::FormatUnsigned<ValueT, BASE>::printReversed(Pt::String& dst, ValueT val, bool uppercase)
{
    // Select the X-Digits set
    const char* XDIGITS = selectXDigits(uppercase);

    // Prepare the destination buffer
    dst.clear();
    dst.resize( FormatUnsigned::countNumberOfUnsignedDigits<ValueT, BASE>(val) );

    // Process the number into a string (reversed)
    Pt::Char* ptr = &dst[0];

    do {
        *ptr++ = XDIGITS[val % BASE];
        val /= BASE;
    } while(val != 0);
}


// Specialization for base 2 (around 20% faster than the generic version)
template <typename ValueT>
struct FormatStringValue::FormatUnsigned<ValueT, 2> : public FormatUnsigned_Common {
    static inline void printReversed(Pt::String& dst, ValueT val, bool uppercase = false);
};


template <typename ValueT>
inline void FormatStringValue::FormatUnsigned<ValueT, 2>::printReversed(Pt::String& dst, ValueT val, bool uppercase)
{
    // Prepare the destination buffer
    dst.clear();
    dst.resize( FormatUnsigned::countNumberOfUnsignedDigits<ValueT, 2>(val) );

    // 16 sets of reversed 4 binary digits
#if 0
    std::cerr << std::endl;
    for(int i = 0; i < 16; ++i) {
        const Pt::String& s = Pt::format("{:04b}", i);
        for(int j = 0; j < 4; ++j) std::cerr << (char) s[3 - j];
        if( !((i + 1) % 8) ) std::cerr << std::endl;
    }
    std::cerr << std::endl;
#endif
    static const char* BIN_DIGITS_R4 =
        "00001000010011000010101001101110"
        "00011001010111010011101101111111";

    // Process the number into a string (reversed)
    Pt::Char* ptr = &dst[0];

    while(val >= 16) {
        Pt::uint32_t idx = (val % 16) * 4;
        val /= 16;
        *ptr++ = BIN_DIGITS_R4[idx++];
        *ptr++ = BIN_DIGITS_R4[idx++];
        *ptr++ = BIN_DIGITS_R4[idx++];
        *ptr++ = BIN_DIGITS_R4[idx  ];
    }

    Pt::uint32_t idx = val * 4;
                 *ptr++ = BIN_DIGITS_R4[idx++];
    if(val >= 2) *ptr++ = BIN_DIGITS_R4[idx++];
    if(val >= 4) *ptr++ = BIN_DIGITS_R4[idx++];
    if(val >= 8) *ptr++ = BIN_DIGITS_R4[idx  ];
}


// Specialization for base 8 (around 18% faster than the generic version)
template <typename ValueT>
struct FormatStringValue::FormatUnsigned<ValueT, 8> : public FormatUnsigned_Common {
    static inline void printReversed(Pt::String& dst, ValueT val, bool uppercase = false);
};


template <typename ValueT>
inline void FormatStringValue::FormatUnsigned<ValueT, 8>::printReversed(Pt::String& dst, ValueT val, bool uppercase)
{
    // Prepare the destination buffer
    dst.clear();
    dst.resize( FormatUnsigned::countNumberOfUnsignedDigits<ValueT, 8>(val) );

    // 64 sets of reversed 2 octal digits
#if 0
    std::cerr << std::endl;
    for(int i = 0; i < 64; ++i) {
        const Pt::String& s = Pt::format("{:02o}", i);
        for(int j = 0; j < 2; ++j) std::cerr << (char) s[1 - j];
        if( !((i + 1) % 16) ) std::cerr << std::endl;
    }
    std::cerr << std::endl;
#endif
    static const char* OCT_DIGITS_R2 =
        "00102030405060700111213141516171"
        "02122232425262720313233343536373"
        "04142434445464740515253545556575"
        "06162636465666760717273747576777";

    // Process the number into a string (reversed)
    Pt::Char* ptr = &dst[0];

    while(val >= 64) {
        Pt::uint32_t idx = (val % 64) * 2;
        val /= 64;
        *ptr++ = OCT_DIGITS_R2[idx++];
        *ptr++ = OCT_DIGITS_R2[idx  ];
    }

    Pt::uint32_t idx = val * 2;
                 *ptr++ = OCT_DIGITS_R2[idx++];
    if(val >= 8) *ptr++ = OCT_DIGITS_R2[idx  ];
}


// Specialization for base 10 (around 18% faster than the generic version)
template <typename ValueT>
struct FormatStringValue::FormatUnsigned<ValueT, 10> : public FormatUnsigned_Common {
    static inline void printReversed(Pt::String& dst, ValueT val, bool uppercase = false);
};


template <typename ValueT>
inline void FormatStringValue::FormatUnsigned<ValueT, 10>::printReversed(Pt::String& dst, ValueT val, bool uppercase)
{
    // Prepare the destination buffer
    dst.clear();
    dst.resize( FormatUnsigned::countNumberOfUnsignedDigits<ValueT, 10>(val) );

    // 100 sets of reversed 2 decimal digits
#if 0
    std::cerr << std::endl;
    for(int i = 0; i < 100; ++i) {
        const Pt::String& s = Pt::format("{:02d}", i);
        for(int j = 0; j < 2; ++j) std::cerr << (char) s[1 - j];
        if( !((i + 1) % 25) ) std::cerr << std::endl;
    }
    std::cerr << std::endl;
#endif
    static const char* DEC_DIGITS_R2 =
        "00102030405060708090011121314151617181910212223242"
        "52627282920313233343536373839304142434445464748494"
        "05152535455565758595061626364656667686960717273747"
        "57677787970818283848586878889809192939495969798999";

    // Process the number into a string (reversed)
    Pt::Char* ptr = &dst[0];

    while(val >= 100) {
        Pt::uint32_t idx = (val % 100) * 2;
        val /= 100;
        *ptr++ = DEC_DIGITS_R2[idx++];
        *ptr++ = DEC_DIGITS_R2[idx  ];
    }

    Pt::uint32_t idx = val * 2;
                  *ptr++ = DEC_DIGITS_R2[idx++];
    if(val >= 10) *ptr++ = DEC_DIGITS_R2[idx  ];
}


// Specialization for base 16 (around 3% faster than the generic version)
template <typename ValueT>
struct FormatStringValue::FormatUnsigned<ValueT, 16> : public FormatUnsigned_Common {
    static inline void printReversed(Pt::String& dst, ValueT val, bool uppercase = false);
};


template <typename ValueT>
inline void FormatStringValue::FormatUnsigned<ValueT, 16>::printReversed(Pt::String& dst, ValueT val, bool uppercase)
{
    // Prepare the destination buffer
    dst.clear();
    dst.resize( FormatUnsigned::countNumberOfUnsignedDigits<ValueT, 16>(val) );

    // 2x 256 sets of reversed 2 hexadecimal digits
#if 0
    // Generate the magic string for base 16 - group every 2 digits
    std::cerr << std::endl;
    for(int i = 0; i < 256; ++i) {
        const Pt::String& s = Pt::format("{:02x}", i);
        for(int j = 0; j < 2; ++j) std::cerr << (char) s[1 - j];
        if( !((i + 1) % 32) ) std::cerr << std::endl;
    }
    std::cerr << std::endl;
    for(int i = 0; i < 256; ++i) {
        const Pt::String& s = Pt::format("{:02X}", i);
        for(int j = 0; j < 2; ++j) std::cerr << (char) s[1 - j];
        if( !((i + 1) % 32) ) std::cerr << std::endl;
    }
    std::cerr << std::endl;
#endif
    static const char* HEX_DIGITS_R2_L =
        "00102030405060708090a0b0c0d0e0f001112131415161718191a1b1c1d1e1f1"
        "02122232425262728292a2b2c2d2e2f203132333435363738393a3b3c3d3e3f3"
        "04142434445464748494a4b4c4d4e4f405152535455565758595a5b5c5d5e5f5"
        "06162636465666768696a6b6c6d6e6f607172737475767778797a7b7c7d7e7f7"
        "08182838485868788898a8b8c8d8e8f809192939495969798999a9b9c9d9e9f9"
        "0a1a2a3a4a5a6a7a8a9aaabacadaeafa0b1b2b3b4b5b6b7b8b9babbbcbdbebfb"
        "0c1c2c3c4c5c6c7c8c9cacbcccdcecfc0d1d2d3d4d5d6d7d8d9dadbdcdddedfd"
        "0e1e2e3e4e5e6e7e8e9eaebecedeeefe0f1f2f3f4f5f6f7f8f9fafbfcfdfefff";
    static const char* HEX_DIGITS_R2_U =
        "00102030405060708090A0B0C0D0E0F001112131415161718191A1B1C1D1E1F1"
        "02122232425262728292A2B2C2D2E2F203132333435363738393A3B3C3D3E3F3"
        "04142434445464748494A4B4C4D4E4F405152535455565758595A5B5C5D5E5F5"
        "06162636465666768696A6B6C6D6E6F607172737475767778797A7B7C7D7E7F7"
        "08182838485868788898A8B8C8D8E8F809192939495969798999A9B9C9D9E9F9"
        "0A1A2A3A4A5A6A7A8A9AAABACADAEAFA0B1B2B3B4B5B6B7B8B9BABBBCBDBEBFB"
        "0C1C2C3C4C5C6C7C8C9CACBCCCDCECFC0D1D2D3D4D5D6D7D8D9DADBDCDDDEDFD"
        "0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFE0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFFF";
    const char* HEX_DIGITS_R2 = uppercase ? HEX_DIGITS_R2_U : HEX_DIGITS_R2_L;

    // Process the number into a string (reversed)
    Pt::Char* ptr = &dst[0];

    while(val >= 256) {
        Pt::uint32_t idx = (val % 256) * 2;
        val /= 256;
        *ptr++ = HEX_DIGITS_R2[idx++];
        *ptr++ = HEX_DIGITS_R2[idx  ];
    }

    Pt::uint32_t idx = val * 2;
                  *ptr++ = HEX_DIGITS_R2[idx++];
    if(val >= 16) *ptr++ = HEX_DIGITS_R2[idx  ];
}


//
// Utilities for formatting (printing) floating-point value
//

// Format/add the decimal point and thousands separator(s) as needed
static inline void finalizePositiveFloatingPointStringFormat(Pt::String& dst, const Pt::String& src, Pt::Char decimalPoint, Pt::Char thousandsSep)
{
    // TODO: Optimize!

    // Simply copy the string if there is no specified decimal point and thousands separator
    if(!decimalPoint && !thousandsSep) {
        dst = src;
        return;
    }

    // Get the source pointers
    const Pt::Char* itSrcBeg = &src[0];
    const Pt::Char* itSrcEnd = itSrcBeg + src.length();
    const Pt::Char* itSrc    = itSrcBeg;

    // Find the decimal point
    const Pt::Char* itSrcDec = 0;
    while(itSrc != itSrcEnd) {
        if( *itSrc == '.' ) {
            itSrcDec = itSrc;
            break;
        }
        if( !isdigit(*itSrc) ) break;
        ++itSrc;
    }

    // Calculate the number of digits before the decimal point
    const size_t numDigitBeforeDec = itSrcDec ? (itSrcDec - itSrcBeg) : (itSrc - itSrcBeg);

    // Check if there is no need to add any thousands separator
    if(!thousandsSep || numDigitBeforeDec <= 3) {
        dst = src;
        if(decimalPoint && numDigitBeforeDec < dst.length()) dst[numDigitBeforeDec] = decimalPoint;
        return;
    }

    // Copy the characters while adding thousands separator(s)
    Pt::uint32_t digitIndex = 3 - (numDigitBeforeDec % 3);

    itSrc = itSrcBeg;
    for(;;) {
        dst += *itSrc++;
        if(itSrc == itSrcDec || itSrc == itSrcEnd) break;
        if( ++digitIndex % 3 == 0) dst += thousandsSep;
    }

    // Add the decimal point
    if(itSrcDec) {
        dst += ( decimalPoint ? decimalPoint : src[numDigitBeforeDec] );
    }

    // Copy the remainder of the characters
    itSrc = itSrcBeg + numDigitBeforeDec + 1;
    while(itSrc != itSrcEnd) dst += *itSrc++;
}


//
// Utilities for writing buffer
//
static inline Pt::Char* fill(Pt::Char* dst, Pt::Char chr, size_t len)
{
    Pt::Char* end = dst + len;

    while(dst != end) *dst++ = chr;

    return dst;
}


static inline Pt::Char* copy(Pt::Char* dst, const Pt::Char* src, size_t len)
{
    const Pt::Char* end = src + len;

    while(src != end) *dst++ = *src++;

    return dst;
}


} // namespace


#endif

