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

#include "FormatString.h"


namespace Pt {


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
// Inline utility functions
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


static inline size_t parseSizeT(const char *p)
{
    size_t v = 0;

    while(*p != '\0') {
        v = (v * 10) + (*p - '0');
        ++p;
    }

    return v;
}


template <typename T>
static inline void printUnsignedRev(Pt::String& dst, T val, Pt::uint8_t base, bool uppercase)
{
    dst.clear();

    static const char* L_DIGITS = "0123456789abcdef";
    static const char* U_DIGITS = "0123456789ABCDEF";
           const char* S_DIGITS = uppercase ? U_DIGITS : L_DIGITS;

    do {
        dst += S_DIGITS[val % base];
        val /= base;
    } while(val != 0);
}


static inline void revUnsignedString(Pt::String& dst, const Pt::String& src, Pt::Char thousandsSep)
{
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
        // Reserve the characters
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
        // Reserve the characters while adding thousands separator(s)
        unsigned int digitIndex = 0;
        for(;;) {
            dst += *srcIt--;
            if(srcIt == srcItEnd) break;
            if(++digitIndex % 3 == 1) dst += thousandsSep;
        }
    }
}


//
// Format-string error
//
FormatStringError::FormatStringError(const std::string& msg)
: std::runtime_error(msg)
{}


FormatStringError::FormatStringError(const char* msg)
: std::runtime_error(msg)
{}


//
// Format-string argument and its corresponding formatter
//
// References:
//     https://en.cppreference.com/w/cpp/utility/format/format
//     https://en.cppreference.com/w/cpp/utility/format/formatter#Standard_format_specification
//     https://en.cppreference.com/w/cpp/chrono/system_clock/formatter#Format_specification
//     https://en.cppreference.com/w/cpp/locale/locale
//     https://en.cppreference.com/w/cpp/locale/num_put
//     https://en.cppreference.com/w/cpp/locale/numpunct
//
// Results verified using:
//     https://fmt.dev/latest/index.html
//     https://github.com/fmtlib/fmt/releases/tag/6.2.0
//     https://github.com/fmtlib/fmt/releases/tag/4.1.0
//
// Performance:
//     About 3x - 5x slower than {fmt} 4.1.0
//     Should still be much faster than sprintf() and std::ostringstream
//
void FormatStringArg::ff_I32(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    // Preparation
    bool         negNum;
    Pt::uint32_t numVal;
    Pt::String   strVal;

    if(_isUInt) {
        negNum = false;
        numVal = _valPOD.u32;
    }
    else {
        negNum = (_valPOD.i32 < 0);
        numVal = negNum ? (-_valPOD.i32) : _valPOD.i32;
    }

    // Handle 'sign' as prefix character
    Pt::String prefixStr;

    if(!fss.sign || fss.sign == '-') {
        if(negNum) {
            prefixStr = '-';
        }
    }
    else if(!fss.sign == '+') {
        if(negNum) prefixStr = '-';
        else       prefixStr = '+';
    }
    else if(!fss.sign == ' ') {
        if(negNum) prefixStr = '-';
        else       prefixStr = ' ';
    }
    else {
        throw FormatStringError("invalid 'sign specifier' in format string");
    }

    // Process as base 2 type
    if( TYPE_IS_B(fss.type) ) {
        // Handle '#' as prefix characters
        if(fss.altForm) {
            fss.altForm = false;
            prefixStr += '0';
            prefixStr += fss.type;
        }
        // Convert to string (in reversed direction)
        printUnsignedRev(strVal, numVal, 2, (fss.type == 'X'));
        // Reverse the string
        revUnsignedString(_valStr, strVal, 0);
    }
    // Process as base 8 type
    else if( TYPE_IS_O(fss.type) ) {
        // Handle '#' as prefix characters
        if(fss.altForm) {
            fss.altForm = false;
            prefixStr += '0';
        }
        // Convert to string (in reversed direction)
        printUnsignedRev(strVal, numVal, 8, (fss.type == 'X'));
        // Reverse the string
        revUnsignedString(_valStr, strVal, 0);
    }
    // Process as base 16 type
    else if( TYPE_IS_X(fss.type) ) {
        // Handle '#' as prefix characters
        if(fss.altForm) {
            fss.altForm = false;
            prefixStr += '0';
            prefixStr += fss.type;
        }
        // Convert to string (in reversed direction)
        printUnsignedRev(strVal, numVal, 16, (fss.type == 'X'));
        // Reverse the string
        revUnsignedString(_valStr, strVal, 0);
    }
    // Process as base 10 type
    else if( TYPE_IS_D(fss.type) ) {
        // Alternate form cannot be used with base 10 type
        if(fss.altForm)
            throw FormatStringError("format specifier '#' requires 'b/B/o/x/X' numeric argument");
        // Convert to string (in reversed direction)
        printUnsignedRev(strVal, numVal, 10, false);
        // Determine the thousands separator
        Pt::Char thousandsSep = 0;
        if(fss.locale) {
            // Get the locale-specific separator if possible
#ifdef PT_WITH_STD_LOCALE
            if(numpunct) thousandsSep = numpunct->thousands_sep();
#endif
            // Otherwise, use the default separator
            if(!thousandsSep) thousandsSep = '.';
        }
        // Reverse the string and add thousands separator as needed
        revUnsignedString(_valStr, strVal, thousandsSep);
    }
    // Invalid type
    else {
        throw FormatStringError("invalid 'type specifier' in format string");
    }

    // Put the prefix characters
    if(fss.zeroPad) {
        if(!prefixStr.empty()) {
            rbf += prefixStr;
            fss.width = ( fss.width > prefixStr.length() ) ? ( fss.width - prefixStr.length() ) : 0;
        }
    }
    else if(!prefixStr.empty()) {
        _valStr = prefixStr + _valStr;
    }

    // Handle '0'
    if(fss.zeroPad) {
        // Handle '0'
        fss.zeroPad = 0;
        if(!fss.align) fss.fill = '0';
    }

    // The default alignment for numeric argument is right
    if(!fss.align) fss.align = '>';

    // Process the generated string
    fss.type   = 's';
    fss.locale = false;
    ff_S(rbf, fss, numpunct);
}


void FormatStringArg::ff_U32(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{ ff_I32(rbf, fss, numpunct); }


void FormatStringArg::ff_I64(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    throw FormatStringError("ff_I64 is not implemented yet!");
}


void FormatStringArg::ff_U64(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{ ff_I64(rbf, fss, numpunct); }


/*
    fill-and-align(optional) sign(optional) #(optional) 0(optional) width(optional) precision(optional) L(optional) type(optional)

    Pt::Char fill;       // fill character
    char     align;      // < > ^
    char     sign;       // + - [space]
    bool     altForm;    // #
    bool     zeroPad;    // 0
    size_t   width;      // minimum field width (default 0)
    size_t   precision;  // floating-point precision (default 6)
    bool     locale;     // use locale-specific formatting
    char     type;       // none/s b B c d o x X a A e E f/F g G p
*/
void FormatStringArg::ff_F(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    throw FormatStringError("ff_F is not implemented yet!");
}


void FormatStringArg::ff_D(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    throw FormatStringError("ff_D is not implemented yet!");
}


void FormatStringArg::ff_LD(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    throw FormatStringError("ff_LD is not implemented yet!");
}


void FormatStringArg::ff_B(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    // Check the specifiers
    if(fss.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(fss.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    // Process as string type
    if( TYPE_IS_S(fss.type) ) {
        // Handle '0'
        if(fss.zeroPad) {
            fss.zeroPad = 0;
            if(!fss.align) fss.fill = '0';
        }
        // Get the locale-specific string if possible
        _valStr.clear();
#ifdef PT_WITH_STD_LOCALE
        if(fss.locale && numpunct) {
            _valStr = _valPOD.b ? numpunct->truename() : numpunct->falsename();
        }
#endif
        // Otherwise, use the default string
        if(_valStr.empty()) {
            _valStr = _valPOD.b ? "true": "false";
        }
        // Process the generated string
        fss.locale = false;
        ff_S(rbf, fss, numpunct);
    }
    // Process as numeric type
    else if( TYPE_IS_C(fss.type) ) {
        _valPOD.u32 = _valPOD.b ? 1 : 0;
        fss.type    = 'd'; // For boolean type 'c' is assumed as 'd'
        ff_U32(rbf, fss, numpunct);
    }
    else if( TYPE_IS_BDOX(fss.type) ) {
        _valPOD.u32 = _valPOD.b ? 1 : 0;
        ff_U32(rbf, fss, numpunct);
    }
    // Invalid type
    else {
        throw FormatStringError("invalid 'type specifier' in format string");
    }
}


void FormatStringArg::ff_P(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    throw FormatStringError("ff_P is not implemented yet!");
}


void FormatStringArg::ff_C(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    // Check the specifiers
    if(fss.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(fss.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    if(fss.zeroPad)
        throw FormatStringError("format specifier '0' requires numeric argument");

    if(fss.locale)
        throw FormatStringError("format specifier 'L' requires numeric/boolean argument");

    // Process as character type
    if( TYPE_IS_C(fss.type) ) {
        _valStr = _valChr;
        ff_S(rbf, fss, numpunct);
    }
    // Process as numeric type
    else if( TYPE_IS_BDOX(fss.type) ) {
        _valPOD.u32 = _valChr;
        ff_U32(rbf, fss, numpunct);
    }
    // Invalid type
    else {
        throw FormatStringError("invalid 'type specifier' in format string");
    }
}


void FormatStringArg::ff_S(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    // Check the specifiers
    if(fss.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(fss.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    if(fss.zeroPad)
        throw FormatStringError("format specifier '0' requires numeric argument");

    if(fss.locale)
        throw FormatStringError("format specifier 'L' requires numeric/boolean argument");

    if( !TYPE_IS_S(fss.type) )
        throw FormatStringError("invalid 'type specifier' in format string");

    // The default alignment for non numeric argument is left
    const char align = fss.align ? fss.align : '<';

    // Calculate string the length
    const size_t strLen = _valStr.length();
    const size_t padLen = fss.width - strLen;

    // Calculate the write pointer and resize the destination buffer
    const size_t    rbfOrgLen = rbf.length();
    const size_t    rbfNewLen = rbfOrgLen + std::max( strLen, fss.width );
          Pt::Char* rbfPtr    = &rbf[0] + rbfOrgLen;

    rbf.resize(rbfNewLen);

    // Center
    if(align == '^') {
        // Pad the string as needed
        if(strLen < fss.width) {
            // Calculate the padding sizes
            const size_t lPadLen = padLen / 2;
            const size_t rPadLen = (lPadLen * 2 == padLen) ? lPadLen : (lPadLen + 1);
            // Left pad the string
            rbfPtr = fill(rbfPtr, fss.fill, lPadLen);
            // Copy the string
            rbfPtr = copy(rbfPtr, _valStr.data(), strLen);
            // Right pad the string
            rbfPtr = fill(rbfPtr, fss.fill, rPadLen);
        }
        // Do not pad the string
        else {
            copy(rbfPtr, _valStr.data(), strLen);
        }
    }
    // Right
    else if(align == '>') {
        // Left pad the string as needed
        if(strLen < fss.width) rbfPtr = fill(rbfPtr, fss.fill, padLen);
        // Copy the string
        copy(rbfPtr, _valStr.data(), strLen);
    }
    // Left
    else if(align == '<') {
        // Copy the string
        rbfPtr = copy(rbfPtr, _valStr.data(), strLen);
        // Right pad the string as needed
        if(strLen < fss.width) fill(rbfPtr, fss.fill, padLen);
    }
    // Invalid
    else {
        throw FormatStringError("invalid 'align specifier' in format string");
    }
}


//
// The main string-formatter class
//
const void FormatString::operator()(Pt::String& resultBuffer) const
{
#define CHECK_FOR_CLOSING_BRACKET() \
    if(*it == '}') {                \
        ++it;                       \
        gotArgFld = true;           \
        continue;                   \
    }                               \
    do {} while(false)              \

    // Format-string specifier
    FormatStringSpec fsSpec;

    // Get the "numpunct" instance (if supported)
#ifdef PT_WITH_STD_LOCALE
    const numpunct_t* numpunct = &std::use_facet< std::numpunct<Pt::Char> >( std::locale() );
#else
    const numpunct_t* numpunct = 0;
#endif

    // Reserve some bytes within the result buffer
    resultBuffer.reserve(512);

    // Variables for processing argument(s)
    bool        gotArgFld = false;

    std::string argIdxStr;
    size_t      argIdxMan = 0;
    size_t      argIdxCnt = 0;
    size_t      argIdxEff;

    bool        gotColon  = false;
    std::string numberStr;

    // Walk through the format characters
    const Pt::Char* it    =      _format.data();
    const Pt::Char* itEnd = it + _format.length();

    while(gotArgFld || it != itEnd) {
        // Check if we have got a complete argument field
        if(gotArgFld) {
            // Get and check the argument index
            if(argIdxStr.empty()) {
                if(argIdxMan) {
                    throw FormatStringError("cannot switch from manual to automatic argument indexing");
                }
                argIdxEff = argIdxCnt++;
            }
            else {
                if(argIdxCnt) {
                    throw FormatStringError("cannot switch from automatic to manual argument indexing");
                }
                argIdxEff = parseSizeT(argIdxStr.c_str());
                argIdxMan = argIdxEff + 1;
            }
            if(!_args || argIdxEff >= _args->size()) {
                throw FormatStringError("'argument index' out of range in format string");
            }
            // Process (format) the argument
            const FormatStringArg& arg = *( (*_args)[argIdxEff] );
            arg(resultBuffer, fsSpec, numpunct);
            // Clear the flags
            gotArgFld = false;
            fsSpec.reset();
            // Break the loop if all the format characters have been processed
            if(it == itEnd) break;
        }
        // Check for '{'
        if(*it == '{') {
            // Check if the next character is also '{'
            ++it;
            if(*it == '{') {
                resultBuffer += *it++;
                continue;
            }
            // Read the 'argument index'
            CHECK_FOR_CLOSING_BRACKET();
            argIdxStr.clear();
            while( isdigit(*it) ) argIdxStr += *it++;
            // Check if the next character is ':'
            CHECK_FOR_CLOSING_BRACKET();
            gotColon = false;
            if(*it == ':') {
                ++it;
                gotColon = true;
            }
            // Read the 'fill' and 'align'
            CHECK_FOR_CLOSING_BRACKET();
            if(*it == '<' || *it == '>' || *it == '^') {
                fsSpec.align = *it++;
            }
            else if(*(it + 1) == '<' || *(it + 1) == '>' || *(it + 1) == '^') {
                fsSpec.fill  = *it++;
                fsSpec.align = *it++;
            }
            // Read the 'sign'
            CHECK_FOR_CLOSING_BRACKET();
            if(*it == '+' || *it == '-' || *it == ' ') {
                fsSpec.sign = *it++;
            }
            // Read the '#'
            CHECK_FOR_CLOSING_BRACKET();
            if(*it == '#') {
                ++it;
                fsSpec.altForm = true;
            }
            // Read the '0'
            CHECK_FOR_CLOSING_BRACKET();
            if(*it == '0') {
                ++it;
                fsSpec.zeroPad = true;
            }
            // Read the 'width'
            CHECK_FOR_CLOSING_BRACKET();
            numberStr.clear();
            while( isdigit(*it) ) numberStr += *it++;
            if(!numberStr.empty()) {
                fsSpec.width = parseSizeT(numberStr.c_str());
            }
            // Check if the next character is '.'
            CHECK_FOR_CLOSING_BRACKET();
            if(*it == '.') {
                // Read the 'precision'
                ++it;
                numberStr.clear();
                while( isdigit(*it) ) numberStr += *it++;
                if(!numberStr.empty()) {
                    fsSpec.precision = parseSizeT(numberStr.c_str());
                }
                else {
                    throw FormatStringError("missing 'precision specifier' in format string");
                }
            }
            // Read the 'locale'
            CHECK_FOR_CLOSING_BRACKET();
            if(*it == 'L') {
                ++it;
                fsSpec.locale = true;
            }
            // Read the 'type'
            CHECK_FOR_CLOSING_BRACKET();
            fsSpec.type = *it++;
            if(fsSpec.type == 'n') { // For backward compatibility with older (draft) C++20 standard
                fsSpec.locale = true;
                fsSpec.type   = 'd';
            }
            // All should be done here
            CHECK_FOR_CLOSING_BRACKET();
            // Error
            if(gotColon)
                throw FormatStringError("invalid character in format string");
            else
                throw FormatStringError("missing '}' in format string");
        }
        // Check for '}'
        else if(*it == '}') {
            // Check if the next character is also '}'
            ++it;
            if(*it == '}') {
                resultBuffer += *it++;
                continue;
            }
            // Error
            throw FormatStringError("unmatched '}' in format string");
        }
        // Other characters
        else {
            resultBuffer += *it++;
        }
    }
}


} // namespace
