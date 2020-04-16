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

// References:
//     https://en.cppreference.com/w/cpp/utility/format/format
//     https://en.cppreference.com/w/cpp/utility/format/formatter#Standard_format_specification
//     https://en.cppreference.com/w/cpp/chrono/system_clock/formatter#Format_specification
//
// Results verified using:
//     https://fmt.dev/latest/index.html
//     https://github.com/fmtlib/fmt/releases/tag/6.2.0
//     https://github.com/fmtlib/fmt/releases/tag/4.1.0

#include <cmath>

#include "FormatStringValue.h"


namespace Pt {


//
// Default values
//
#define FLOAT_LOWER_INF             "inf"
#define FLOAT_UPPER_INF             "INF"
#define FLOAT_LOWER_NAN             "nan"
#define FLOAT_UPPER_NAN             "NAN"

#define DEFAULT_TRUE_NAME           "true"
#define DEFAULT_FALSE_NAME          "false"

#define DEFAULT_DECIMAL_POINT       '.'
#define DEFAULT_THOUSANDS_SEPARATOR ','

#define DEFAULT_PRECISION           6


//
// Utility macros
//
#define TYPE_IS_N(T)    ( !T                                          )

#define TYPE_IS_S(T)    ( !T || ( T &&   (T == 's')                 ) )
#define TYPE_IS_C(T)    ( !T || ( T &&   (T == 'c')                 ) )

#define TYPE_IS_B(T)    (       ( T && ( (T == 'b') || (T == 'B') ) ) )
#define TYPE_IS_D(T)    ( !T || ( T &&   (T == 'd')                 ) )
#define TYPE_IS_O(T)    (       ( T &&   (T == 'o')                 ) )
#define TYPE_IS_X(T)    (       ( T && ( (T == 'x') || (T == 'X') ) ) )

#define TYPE_IS_A(T)    (       ( T && ( (T == 'a') || (T == 'A') ) ) )
#define TYPE_IS_E(T)    (       ( T && ( (T == 'e') || (T == 'E') ) ) )
#define TYPE_IS_F(T)    (       ( T && ( (T == 'f') || (T == 'F') ) ) )
#define TYPE_IS_G(T)    (       ( T && ( (T == 'g') || (T == 'G') ) ) )

#define TYPE_IS_P(T)    ( !T || ( T &&   (T == 'p')                 ) )

#define TYPE_IS_BDOX(T) ( T && ( (T == 'b') || (T == 'B') || (T == 'd') || (T == 'o') || (T == 'x') || (T == 'X') ) )
#define TYPE_IS_AEFG(T) ( T && ( (T == 'a') || (T == 'A') || (T == 'e') || (T == 'E') || (T == 'f') || (T == 'F') || (T == 'g') || (T == 'G') ) )


//
// Utility templates
//
template <typename ValueT>
struct SelectInt;

template<>
struct SelectInt<Pt::int32_t> {
    typedef Pt::int32_t  SignedT;
    typedef Pt::uint32_t UnsignedT;

    static inline Pt::int32_t  selectSigned  (Pt::int32_t  i32, Pt::int64_t ) { return i32; }
    static inline Pt::uint32_t selectUnsigned(Pt::uint32_t u32, Pt::uint64_t) { return u32; }
};

template<>
struct SelectInt<Pt::int64_t> {
    typedef Pt::int64_t  SignedT;
    typedef Pt::uint64_t UnsignedT;

    static inline Pt::int64_t  selectSigned  (Pt::int32_t,  Pt::int64_t  i64) { return i64; }
    static inline Pt::uint64_t selectUnsigned(Pt::uint32_t, Pt::uint64_t u64) { return u64; }
};


template <typename T, int base>
struct PrintUnsigned {
    static inline void Reversed(Pt::String& dst, T val, bool uppercase)
    {
        const char* XDIGITS = FormatStringValue::selectXDigits(uppercase);

        dst.clear();

        do {
            dst += XDIGITS[val % base];
            val /= base;
        } while(val != 0);
    }
};

template <typename T>
struct PrintUnsigned<T, 2> {
    static inline void Reversed(Pt::String& dst, T val, bool)
    {
        static const char* BIN_DIGITS_R4 =
            "00001000010011000010101001101110"
            "00011001010111010011101101111111";

        dst.clear();

        while(val >= 16) {
            Pt::uint32_t idx = (val % 16) * 4;
            val /= 16;
            dst += BIN_DIGITS_R4[idx++];
            dst += BIN_DIGITS_R4[idx++];
            dst += BIN_DIGITS_R4[idx++];
            dst += BIN_DIGITS_R4[idx  ];
        }

        Pt::uint32_t idx = val * 4;
                     dst += BIN_DIGITS_R4[idx++];
        if(val >= 2) dst += BIN_DIGITS_R4[idx++];
        if(val >= 4) dst += BIN_DIGITS_R4[idx++];
        if(val >= 8) dst += BIN_DIGITS_R4[idx  ];
    }
};

template <typename T>
struct PrintUnsigned<T, 8> {
    static inline void Reversed(Pt::String& dst, T val, bool)
    {
        static const char* OCT_DIGITS_R2 =
            "00102030405060700111213141516171"
            "02122232425262720313233343536373"
            "04142434445464740515253545556575"
            "06162636465666760717273747576777";

        dst.clear();

        while(val >= 64) {
            Pt::uint32_t idx = (val % 64) * 2;
            val /= 64;
            dst += OCT_DIGITS_R2[idx++];
            dst += OCT_DIGITS_R2[idx  ];
        }

        Pt::uint32_t idx = val * 2;
                     dst += OCT_DIGITS_R2[idx++];
        if(val >= 8) dst += OCT_DIGITS_R2[idx  ];
    }
};

template <typename T>
struct PrintUnsigned<T, 10> {
    static inline void Reversed(Pt::String& dst, T val, bool)
    {
        static const char* DEC_DIGITS_R2 =
            "00102030405060708090011121314151617181910212223242"
            "52627282920313233343536373839304142434445464748494"
            "05152535455565758595061626364656667686960717273747"
            "57677787970818283848586878889809192939495969798999";

        dst.clear();

        while(val >= 100) {
            Pt::uint32_t idx = (val % 100) * 2;
            val /= 100;
            dst += DEC_DIGITS_R2[idx++];
            dst += DEC_DIGITS_R2[idx  ];
        }

        Pt::uint32_t idx = val * 2;
                      dst += DEC_DIGITS_R2[idx++];
        if(val >= 10) dst += DEC_DIGITS_R2[idx  ];
    }
};

template <typename T>
struct PrintUnsigned<T, 16> {
    static inline void Reversed(Pt::String& dst, T val, bool uppercase)
    {
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

        dst.clear();

        while(val >= 256) {
            Pt::uint32_t idx = (val % 256) * 2;
            val /= 256;
            dst += HEX_DIGITS_R2[idx++];
            dst += HEX_DIGITS_R2[idx  ];
        }

        Pt::uint32_t idx = val * 2;
                      dst += HEX_DIGITS_R2[idx++];
        if(val >= 16) dst += HEX_DIGITS_R2[idx  ];
    }
};


//
// Utility functions
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


static inline void revUnsignedString(Pt::String& dst, const Pt::String& src, Pt::Char thousandsSep)
{
    // TODO: Optimize!

    // Get the source length
    const size_t srcLen = src.length();

    // Get the source pointers
    const Pt::Char* srcIt    = &src[srcLen - 1];
    const Pt::Char* srcItEnd = srcIt - srcLen;

    // Process without using thousands separator
    if(!thousandsSep || srcLen <= 3) {
        // Resize the destination buffer
        dst.clear();
        dst.reserve(srcLen);
        // Reverse the characters
        while(srcIt != srcItEnd) dst += *srcIt--;
    }
    // Process using thousands separator(s)
    else {
        // Calculate the number of thousands separator(s)
        const size_t sepCnt = (srcLen + 2) / 3 - 1;
        const size_t dstLen = src.length() + sepCnt;
        // Resize the destination buffer
        dst.clear();
        dst.reserve(dstLen);
        // Reverse the characters while adding thousands separator(s)
        Pt::uint32_t digitIndex = 3 - (srcLen % 3);
        for(;;) {
            dst += *srcIt--;
            if(srcIt == srcItEnd) break;
            if( ++digitIndex % 3 == 0) dst += thousandsSep;
        }
    }
}


static inline void formatFPString(Pt::String& dst, const Pt::String& src, Pt::Char decimalPoint, Pt::Char thousandsSep)
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
// Format-string value and its corresponding formatter
//
const char FormatStringValue::XDIGITS_LOWER[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

const char FormatStringValue::XDIGITS_UPPER[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};


template <typename ValueT> inline
void FormatStringValue::ff_IXX(Pt::String& resBuff, Rule& rule, const numpunct_t* numpunct) const
{
    // TODO: Optimize!

    // Preparation
    typedef typename SelectInt<ValueT>::SignedT   SignedT;
    typedef typename SelectInt<ValueT>::UnsignedT UnsignedT;

    bool      negNum;
    UnsignedT numVal;

    if(_isUnsigned) {
        negNum = false;
        numVal = SelectInt<ValueT>::selectUnsigned(_valPOD.u32, _valPOD.u64);
    }
    else {
        const SignedT sigVal = SelectInt<ValueT>::selectSigned(_valPOD.i32, _valPOD.i64);

        negNum = (sigVal < 0);
        numVal = negNum ? -sigVal : sigVal;
    }

    // Handle 'sign' as a prefix character
    Pt::String prefixStr;

    if(!rule.sign || rule.sign == '-') {
        if(negNum) {
            prefixStr = '-';
        }
    }
    else if(rule.sign == '+') {
        if(negNum) prefixStr = '-';
        else       prefixStr = '+';
    }
    else if(rule.sign == ' ') {
        if(negNum) prefixStr = '-';
        else       prefixStr = ' ';
    }
    else {
        throw FormatStringError("invalid 'sign specifier' in format string");
    }

    // For backward compatibility with older (draft) C++20 standard
    if(rule.type == 'n') {
        rule.locale = true;
        rule.type   = 'd';
    }

    // Temporary result buffers
    Pt::String strVal;
    Pt::String tmpResBuff;

    // Process as base 2 type
    if( TYPE_IS_B(rule.type) ) {
        // Handle '#' as prefix characters
        if(rule.altForm) {
            rule.altForm = false;
            prefixStr += '0';
            prefixStr += rule.type;
        }
        // Convert to string (in reversed direction)
        PrintUnsigned<UnsignedT, 2>::Reversed(strVal, numVal, false);
        // Reverse the string
        revUnsignedString(tmpResBuff, strVal, 0);
    }
    // Process as base 8 type
    else if( TYPE_IS_O(rule.type) ) {
        // Handle '#' as prefix characters
        if(rule.altForm) {
            rule.altForm = false;
            prefixStr += '0';
        }
        // Convert to string (in reversed direction)
        PrintUnsigned<UnsignedT, 8>::Reversed(strVal, numVal, false);
        // Reverse the string
        revUnsignedString(tmpResBuff, strVal, 0);
    }
    // Process as base 16 type
    else if( TYPE_IS_X(rule.type) ) {
        // Handle '#' as prefix characters
        if(rule.altForm) {
            rule.altForm = false;
            prefixStr += '0';
            prefixStr += rule.type;
        }
        // Convert to string (in reversed direction)
        PrintUnsigned<UnsignedT, 16>::Reversed(strVal, numVal, rule.type == 'X');
        // Reverse the string
        revUnsignedString(tmpResBuff, strVal, 0);
    }
    // Process as base 10 type
    else if( TYPE_IS_D(rule.type) ) {
        // Alternate form cannot be used with base 10 type
        if(rule.altForm)
            throw FormatStringError("format specifier '#' requires 'b/B/o/x/X' numeric argument");
        // Convert to string (in reversed direction)
        PrintUnsigned<UnsignedT, 10>::Reversed(strVal, numVal, false);
        // Determine the thousands separator
        Pt::Char thousandsSep = 0;
        if(rule.locale) {
            // Get the locale-specific thousands separator if possible
#ifdef PT_WITH_STD_LOCALE
            if(numpunct) thousandsSep = numpunct->thousands_sep();
#endif
            // Otherwise, use the default thousands separator
            if(!thousandsSep) thousandsSep = DEFAULT_THOUSANDS_SEPARATOR;
        }
        // Reverse the string and add thousands separator as needed
        revUnsignedString(tmpResBuff, strVal, thousandsSep);
    }
    // Invalid type
    else {
        throw FormatStringError("invalid 'type specifier' in format string");
    }

    // Put the prefix characters
    if(rule.zeroPad) {
        if(!prefixStr.empty()) {
            if(!rule.align) {
                resBuff += prefixStr;
                rule.width = ( rule.width > prefixStr.length() ) ? ( rule.width - prefixStr.length() ) : 0;
            }
            else {
                tmpResBuff = prefixStr + tmpResBuff;
            }
        }
    }
    else if(!prefixStr.empty()) {
        tmpResBuff = prefixStr + tmpResBuff;
    }

    // Handle '0'
    if(rule.zeroPad) {
        // Handle '0'
        rule.zeroPad = false;
        if(!rule.align) rule.fill = '0';
    }

    // The default alignment for numeric argument is right
    if(!rule.align) rule.align = '>';

    // Process the generated string
    Rule r(rule.fill, rule.align, rule.width, 's');
    FormatStringValue(tmpResBuff).ff_S(resBuff, r, numpunct);
}


void FormatStringValue::ff_I32(Pt::String& resBuff, Rule& rule, const numpunct_t* numpunct) const
{ ff_IXX<Pt::int32_t>(resBuff, rule, numpunct); }


void FormatStringValue::ff_I64(Pt::String& resBuff, Rule& rule, const numpunct_t* numpunct) const
{ ff_IXX<Pt::int64_t>(resBuff, rule, numpunct); }


void FormatStringValue::ff_LD(Pt::String& resBuff, Rule& rule, const numpunct_t* numpunct) const
{
    // TODO: Optimize!

    // Preparation
    const bool        negNum = (_valPOD.ld < 0.0L);
    const long double numVal = negNum ? -_valPOD.ld : _valPOD.ld;

    // Temporary result buffer
    Pt::String tmpResBuff;

    // Process Inf and NaN
    if(isinf(numVal)) {
        if(Pt::isupper(rule.type)) tmpResBuff = FLOAT_UPPER_INF;
        else                       tmpResBuff = FLOAT_LOWER_INF;
    }
    else if(isnan(numVal)) {
        if(Pt::isupper(rule.type)) tmpResBuff = FLOAT_UPPER_NAN;
        else                       tmpResBuff = FLOAT_LOWER_NAN;
    }

    // Handle 'sign' as a prefix character
    Pt::String prefixStr;

    if(!rule.sign || rule.sign == '-') {
        if(negNum) {
            prefixStr = '-';
        }
    }
    else if(rule.sign == '+') {
        if(negNum) prefixStr = '-';
        else       prefixStr = '+';
    }
    else if(rule.sign == ' ') {
        if(negNum) prefixStr = '-';
        else       prefixStr = ' ';
    }
    else {
        throw FormatStringError("invalid 'sign specifier' in format string");
    }

    // Check if we have Inf or NaN
    if(!tmpResBuff.empty()) {
        // The default alignment for numeric argument is right
        if(!rule.align) rule.align = '>';
        // Process as string type
        Rule r(rule.fill, rule.align, rule.width, 's');
        FormatStringValue(prefixStr + tmpResBuff).ff_S(resBuff, r, numpunct);
        // We are done here
        return;
    }

    // Handle 'a' and 'A'
    if( TYPE_IS_A(rule.type) ) {
        prefixStr += '0';
        prefixStr += (rule.type == 'A') ? 'X' : 'x';
    }

    // Determine the decimal point
    Pt::Char decimalPoint = 0;
    if(rule.locale) {
        // Get the locale-specific decimal point if possible
#ifdef PT_WITH_STD_LOCALE
        if(numpunct) decimalPoint = numpunct->decimal_point();
#endif
        // Otherwise, use the default decimal point
        if(!decimalPoint) decimalPoint = DEFAULT_DECIMAL_POINT;
    }

    // Determine the thousands separator
    Pt::Char thousandsSep = 0;
    if(rule.locale) {
        // Get the locale-specific thousands separator if possible
#ifdef PT_WITH_STD_LOCALE
        if(numpunct) thousandsSep = numpunct->thousands_sep();
#endif
        // Otherwise, use the default thousands separator
        if(!thousandsSep) thousandsSep = DEFAULT_THOUSANDS_SEPARATOR;
    }

    // Format the number
    if(  rule.precision == (size_t) -1 ) rule.precision = DEFAULT_PRECISION;
    if( !rule.type                     ) rule.type      = 'g';

    Pt::String strFP;
    formatPositiveFP(strFP, numVal, rule.precision, rule.altForm, rule.type);

    // Handle the decimal point and thousands separator
    formatFPString(tmpResBuff, strFP, decimalPoint, thousandsSep);

    // Put the prefix characters
    if(rule.zeroPad) {
        if(!prefixStr.empty()) {
            if(!rule.align) {
                resBuff += prefixStr;
                rule.width = ( rule.width > prefixStr.length() ) ? ( rule.width - prefixStr.length() ) : 0;
            }
            else {
                tmpResBuff = prefixStr + tmpResBuff;
            }
        }
    }
    else if(!prefixStr.empty()) {
        tmpResBuff = prefixStr + tmpResBuff;
    }

    // Handle '0'
    if(rule.zeroPad) {
        rule.zeroPad = false;
        if(!rule.align) rule.fill = '0';
    }

    // The default alignment for numeric argument is right
    if(!rule.align) rule.align = '>';

    // Process as string type
    Rule r(rule.fill, rule.align, rule.width, 's');
    FormatStringValue(tmpResBuff).ff_S(resBuff, r, numpunct);
}


void FormatStringValue::ff_B(Pt::String& resBuff, Rule& rule, const numpunct_t* numpunct) const
{
    // Check the specifiers
    if(rule.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(rule.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    // Process as string type
    if( TYPE_IS_S(rule.type) ) {
        // Handle '0'
        if(rule.zeroPad) {
            rule.zeroPad = false;
            if(!rule.align) rule.fill = '0';
        }
        // Get the locale-specific string if possible
        Pt::String boolName;
#ifdef PT_WITH_STD_LOCALE
        if(rule.locale && numpunct) {
            boolName = _valPOD.b ? numpunct->truename() : numpunct->falsename();
        }
#endif
        // Otherwise, use the default string
        if(boolName.empty()) {
            boolName = _valPOD.b ? DEFAULT_TRUE_NAME : DEFAULT_FALSE_NAME;
        }
        // Process the generated string
        Rule r(rule.fill, rule.align, rule.width, rule.type);
        FormatStringValue(boolName).ff_S(resBuff, r, numpunct);
    }
    // Process as numeric type
    else if( TYPE_IS_C(rule.type) ) {
        Rule r(rule.fill, rule.align, rule.width, 'd', rule.zeroPad); // For boolean, type 'c' is considered the same as type 'd'
        FormatStringValue((Pt::uint32_t) _valPOD.b ? 1 : 0).ff_I32(resBuff, r, numpunct);
    }
    else if( TYPE_IS_BDOX(rule.type) ) {
        Rule r(rule.fill, rule.align, rule.width, rule.type, rule.zeroPad);
        FormatStringValue((Pt::uint32_t) _valPOD.b ? 1 : 0).ff_I32(resBuff, r, numpunct);
    }
    // Invalid type
    else {
        throw FormatStringError("invalid 'type specifier' in format string");
    }
}


void FormatStringValue::ff_P(Pt::String& resBuff, Rule& rule, const numpunct_t* numpunct) const
{
    // Check the specifiers
    if(rule.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(rule.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    if(rule.zeroPad)
        throw FormatStringError("format specifier '0' requires numeric argument");

    if(rule.locale)
        throw FormatStringError("format specifier 'L' requires numeric/boolean argument");

    if( !TYPE_IS_P(rule.type) )
        throw FormatStringError("invalid 'type specifier' in format string");

    // Process as numeric type
    Rule r(rule.fill, rule.align, std::max(rule.width, sizeof(void*)), 'x', false, true);
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
    FormatStringValue((Pt::uint64_t) _valPOD.p).ff_I64(resBuff, r, numpunct);
#else
    FormatStringValue((Pt::uint32_t) _valPOD.p).ff_I32(resBuff, r, numpunct);
#endif
}


void FormatStringValue::ff_C(Pt::String& resBuff, Rule& rule, const numpunct_t* numpunct) const
{
    // Check the specifiers
    if(rule.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(rule.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    if(rule.zeroPad)
        throw FormatStringError("format specifier '0' requires numeric argument");

    if(rule.locale)
        throw FormatStringError("format specifier 'L' requires numeric/boolean argument");

    // Process as character type
    if( TYPE_IS_C(rule.type) ) {
        Rule r(rule.fill, rule.align, rule.width, rule.type);
        FormatStringValue(Pt::String(1, _valChr)).ff_S(resBuff, r, numpunct);
    }
    // Process as numeric type
    else if( TYPE_IS_BDOX(rule.type) ) {
        Rule r(rule.fill, rule.align, rule.width, rule.type);
        FormatStringValue((Pt::uint32_t) _valChr).ff_I32(resBuff, r, numpunct);
    }
    // Invalid type
    else {
        throw FormatStringError("invalid 'type specifier' in format string");
    }
}


void FormatStringValue::ff_S(Pt::String& resBuff, Rule& rule, const numpunct_t* numpunct) const
{
    // Check the specifiers
    if(rule.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(rule.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    if(rule.zeroPad)
        throw FormatStringError("format specifier '0' requires numeric argument");

    if(rule.locale)
        throw FormatStringError("format specifier 'L' requires numeric/boolean argument");

    if( !TYPE_IS_S(rule.type) )
        throw FormatStringError("invalid 'type specifier' in format string");

    // The default alignment for non numeric argument is left
    const char align = rule.align ? rule.align : '<';

    // Calculate string the length
    const size_t strLen = _valStr.length();
    const size_t padLen = rule.width - strLen;

    // Resize the destination buffer
    const size_t resBuffOrgLen = resBuff.length();
    const size_t resBuffNewLen = resBuffOrgLen + std::max( strLen, rule.width );

    resBuff.resize(resBuffNewLen);

    // Get the write pointer
    Pt::Char* resBuffPtr = &resBuff[0] + resBuffOrgLen;

    // Center
    if(align == '^') {
        // Pad the string as needed
        if(strLen < rule.width) {
            // Calculate the padding sizes
            const size_t lPadLen = padLen / 2;
            const size_t rPadLen = (lPadLen * 2 == padLen) ? lPadLen : (lPadLen + 1);
            // Left pad the string
            resBuffPtr = fill(resBuffPtr, rule.fill, lPadLen);
            // Copy the string
            resBuffPtr = copy(resBuffPtr, _valStr.data(), strLen);
            // Right pad the string
            resBuffPtr = fill(resBuffPtr, rule.fill, rPadLen);
        }
        // Do not pad the string
        else {
            copy(resBuffPtr, _valStr.data(), strLen);
        }
    }
    // Right
    else if(align == '>') {
        // Left pad the string as needed
        if(strLen < rule.width) resBuffPtr = fill(resBuffPtr, rule.fill, padLen);
        // Copy the string
        copy(resBuffPtr, _valStr.data(), strLen);
    }
    // Left
    else if(align == '<') {
        // Copy the string
        resBuffPtr = copy(resBuffPtr, _valStr.data(), strLen);
        // Right pad the string as needed
        if(strLen < rule.width) fill(resBuffPtr, rule.fill, padLen);
    }
    // Invalid
    else {
        throw FormatStringError("invalid 'align specifier' in format string");
    }
}


} // namespace
