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


template <typename T>
static inline void printUnsignedRev(Pt::String& dst, T val, Pt::uint8_t base, const char* xdigits)
{
    // TODO: Optimize!

    dst.clear();

    do {
        dst += xdigits[val % base];
        val /= base;
    } while(val != 0);
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
void FormatStringValue::ff_IXX(Pt::String &resBuff, Rule& rule, const numpunct_t* numpunct) const
{
    // TODO: Optimize!

    // Preparation
    typedef typename SelectInt<ValueT>::SignedT   SignedT;
    typedef typename SelectInt<ValueT>::UnsignedT UnsignedT;

    bool       negNum;
    UnsignedT  numVal;
    Pt::String strVal;

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

    // Process as base 2 type
    if( TYPE_IS_B(rule.type) ) {
        // Handle '#' as prefix characters
        if(rule.altForm) {
            rule.altForm = false;
            prefixStr += '0';
            prefixStr += rule.type;
        }
        // Convert to string (in reversed direction)
        printUnsignedRev(strVal, numVal, 2, selectXDigits(false));
        // Reverse the string
        revUnsignedString(_valStr, strVal, 0);
    }
    // Process as base 8 type
    else if( TYPE_IS_O(rule.type) ) {
        // Handle '#' as prefix characters
        if(rule.altForm) {
            rule.altForm = false;
            prefixStr += '0';
        }
        // Convert to string (in reversed direction)
        printUnsignedRev(strVal, numVal, 8, selectXDigits(false));
        // Reverse the string
        revUnsignedString(_valStr, strVal, 0);
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
        printUnsignedRev(strVal, numVal, 16, selectXDigits(rule.type == 'X'));
        // Reverse the string
        revUnsignedString(_valStr, strVal, 0);
    }
    // Process as base 10 type
    else if( TYPE_IS_D(rule.type) ) {
        // Alternate form cannot be used with base 10 type
        if(rule.altForm)
            throw FormatStringError("format specifier '#' requires 'b/B/o/x/X' numeric argument");
        // Convert to string (in reversed direction)
        printUnsignedRev(strVal, numVal, 10, selectXDigits(false));
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
        revUnsignedString(_valStr, strVal, thousandsSep);
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
                _valStr = prefixStr + _valStr;
            }
        }
    }
    else if(!prefixStr.empty()) {
        _valStr = prefixStr + _valStr;
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
    rule.sign   = 0;
    rule.locale = false;
    rule.type   = 's';
    ff_S(resBuff, rule, numpunct);
}


void FormatStringValue::ff_I32(Pt::String &resBuff, Rule& rule, const numpunct_t* numpunct) const
{ ff_IXX<Pt::int32_t>(resBuff, rule, numpunct); }


void FormatStringValue::ff_I64(Pt::String &resBuff, Rule& rule, const numpunct_t* numpunct) const
{ ff_IXX<Pt::int64_t>(resBuff, rule, numpunct); }


void FormatStringValue::ff_LD(Pt::String &resBuff, Rule& rule, const numpunct_t* numpunct) const
{
    // TODO: Optimize!

    // Preparation
    const bool        negNum = (_valPOD.ld < 0.0L);
    const long double numVal = negNum ? -_valPOD.ld : _valPOD.ld;

    // Process Inf and NaN
    _valStr.clear();

    if(isinf(numVal)) {
        if(Pt::isupper(rule.type)) _valStr = FLOAT_UPPER_INF;
        else                       _valStr = FLOAT_LOWER_INF;
    }
    else if(isnan(numVal)) {
        if(Pt::isupper(rule.type)) _valStr = FLOAT_UPPER_NAN;
        else                       _valStr = FLOAT_LOWER_NAN;
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

    if(!_valStr.empty()) {
        // The default alignment for numeric argument is right
        if(!rule.align) rule.align = '>';
        // Process as string type
        rule.sign    = 0;
        rule.altForm = false;
        rule.zeroPad = false;
        rule.locale  = false;
        rule.type    = 's';
        ff_S(resBuff, rule, numpunct);
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
    formatFPString(_valStr, strFP, decimalPoint, thousandsSep);

    // Put the prefix characters
    if(rule.zeroPad) {
        if(!prefixStr.empty()) {
            if(!rule.align) {
                resBuff += prefixStr;
                rule.width = ( rule.width > prefixStr.length() ) ? ( rule.width - prefixStr.length() ) : 0;
            }
            else {
                _valStr = prefixStr + _valStr;
            }
        }
    }
    else if(!prefixStr.empty()) {
        _valStr = prefixStr + _valStr;
    }

    // Handle '0'
    if(rule.zeroPad) {
        rule.zeroPad = false;
        if(!rule.align) rule.fill = '0';
    }

    // The default alignment for numeric argument is right
    if(!rule.align) rule.align = '>';

    // Process as string type
    rule.sign    = 0;
    rule.altForm = false;
    rule.locale  = false;
    rule.type    = 's';
    ff_S(resBuff, rule, numpunct);
}


void FormatStringValue::ff_B(Pt::String &resBuff, Rule& rule, const numpunct_t* numpunct) const
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
        _valStr.clear();
#ifdef PT_WITH_STD_LOCALE
        if(rule.locale && numpunct) {
            _valStr = _valPOD.b ? numpunct->truename() : numpunct->falsename();
        }
#endif
        // Otherwise, use the default string
        if(_valStr.empty()) {
            _valStr = _valPOD.b ? DEFAULT_TRUE_NAME : DEFAULT_FALSE_NAME;
        }
        // Process the generated string
        rule.locale = false;
        ff_S(resBuff, rule, numpunct);
    }
    // Process as numeric type
    else if( TYPE_IS_C(rule.type) ) {
        _valPOD.u32 = _valPOD.b ? 1 : 0;
        rule.type    = 'd'; // For boolean, type 'c' is considered the same as type 'd'
        ff_I32(resBuff, rule, numpunct);
    }
    else if( TYPE_IS_BDOX(rule.type) ) {
        _valPOD.u32 = _valPOD.b ? 1 : 0;
        ff_I32(resBuff, rule, numpunct);
    }
    // Invalid type
    else {
        throw FormatStringError("invalid 'type specifier' in format string");
    }
}


void FormatStringValue::ff_P(Pt::String &resBuff, Rule& rule, const numpunct_t* numpunct) const
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
    rule.altForm = true;
    rule.width   = std::max( rule.width, sizeof(void*) );
    rule.type    = 'x';
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
    _valPOD.u64 = (Pt::uint64_t) _valPOD.p;
    ff_I64(resBuff, rule, numpunct);
#else
    _valPOD.u32 = (Pt::uint32_t) _valPOD.p;
    ff_I32(resBuff, rule, numpunct);
#endif
}


void FormatStringValue::ff_C(Pt::String &resBuff, Rule& rule, const numpunct_t* numpunct) const
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
        _valStr = _valChr;
        ff_S(resBuff, rule, numpunct);
    }
    // Process as numeric type
    else if( TYPE_IS_BDOX(rule.type) ) {
        _valPOD.u32 = _valChr;
        ff_I32(resBuff, rule, numpunct);
    }
    // Invalid type
    else {
        throw FormatStringError("invalid 'type specifier' in format string");
    }
}


void FormatStringValue::ff_S(Pt::String &resBuff, Rule& rule, const numpunct_t* numpunct) const
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
