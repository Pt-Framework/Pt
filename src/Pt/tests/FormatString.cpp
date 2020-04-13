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
#define TYPE_IS_N(T)     ( !T )

#define TYPE_IS_P(T)     ( !T || (T && T == 'p') )
#define TYPE_IS_C(T)     ( !T || (T && T == 'c') )
#define TYPE_IS_S(T)     ( !T || (T && T == 's') )

#define TYPE_IS_BDOX(T)  ( T && ( (T == 'b') || (T == 'B') || (T == 'd') || (T == 'o') || (T == 'x') || (T == 'X') ) )
#define TYPE_IS_BCDOX(T) ( T && ( (T == 'b') || (T == 'B') || (T == 'c') || (T == 'd') || (T == 'o') ||  (T == 'x') || (T == 'X') ) )
#define TYPE_IS_AEFG(T)  ( T && ( (T == 'a') || (T == 'A') || (T == 'e') || (T == 'E') || (T == 'f') ||  (T == 'F') || (T == 'g') || (T == 'G') ) )


//
// Inline utility functions
//
static inline unsigned int parseUInt(const char *p)
{
    unsigned int v = 0;

    while(*p != '\0') {
        v = (v * 10) + (*p - '0');
        ++p;
    }

    return v;
}

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
//     https://github.com/fmtlib/fmt/releases/tag/4.1.0
//
void FormatStringArg::ff_I8(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_I8 Not implemented yet!");
}


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
void FormatStringArg::ff_U8(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
/*
#define TYPE_IS_N(T)     ( !T )

#define TYPE_IS_P(T)     ( !T || (T && T == 'p') )
#define TYPE_IS_C(T)     ( !T || (T && T == 'c') )
#define TYPE_IS_S(T)     ( !T || (T && T == 's') )

#define TYPE_IS_BDOX(T)  ( T && ( (T == 'b') || (T == 'B') || (T == 'd') || (T == 'o') || (T == 'x') || (T == 'X') ) )
#define TYPE_IS_BCDOX(T) ( T && ( (T == 'b') || (T == 'B') || (T == 'c') || (T == 'd') || (T == 'o') ||  (T == 'x') || (T == 'X') ) )
#define TYPE_IS_AEFG(T)  ( T && ( (T == 'a') || (T == 'A') || (T == 'e') || (T == 'E') || (T == 'f') ||  (T == 'F') || (T == 'g') || (T == 'G') ) )

 * */


    throw FormatStringError("ff_U8 Not implemented yet!");
}


void FormatStringArg::ff_I16(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_I16 Not implemented yet!");
}


void FormatStringArg::ff_U16(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_U16 Not implemented yet!");
}


void FormatStringArg::ff_I32(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_I32 Not implemented yet!");
}


void FormatStringArg::ff_U32(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_U32 Not implemented yet!");
}


void FormatStringArg::ff_I64(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_I64 Not implemented yet!");
}


void FormatStringArg::ff_U64(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_U64 Not implemented yet!");
}


void FormatStringArg::ff_F(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_F Not implemented yet!");
}


void FormatStringArg::ff_D(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_D Not implemented yet!");
}


void FormatStringArg::ff_LD(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_LD Not implemented yet!");
}


void FormatStringArg::ff_B(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    // Check the specifiers
    if(fss.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(fss.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    if(fss.locale)
        throw FormatStringError("format specifier 'L' is not supported yet");

    // Process according the required type
    if( TYPE_IS_S(fss.type) ) {
        if(fss.zeroPad) {
            fss.zeroPad = 0;
            if(!fss.align) fss.fill = '0';
        }
        _valStr = _valPOD.b ? numpunct.truename() : numpunct.falsename();
        ff_S(rbf, fss, numpunct);
    }
    else if( TYPE_IS_BCDOX(fss.type) ) {
        _valPOD.u8 = _valPOD.b ? 1 : 0;
        ff_U8(rbf, fss, numpunct);
    }
    else {
        throw FormatStringError("invalid 'type' in format string");
    }
}


void FormatStringArg::ff_P(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_P Not implemented yet!");
}


void FormatStringArg::ff_S(Pt::String &rbf, FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    // Check the specifiers
    if(fss.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(fss.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    if(fss.zeroPad)
        throw FormatStringError("format specifier '0' requires numeric argument");

    if(fss.locale)
        throw FormatStringError("format specifier 'L' is not supported yet");

    if( !TYPE_IS_S(fss.type) )
        throw FormatStringError("invalid 'type' in format string");

    // Get the alignment
    const char align = fss.align ? fss.align : '<'; // The default alignment for string is left

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
        throw FormatStringError("invalid 'align' in format string");
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

    // Get the "numpunct" instance
    const std::numpunct<Pt::Char>& numpunct = std::use_facet< std::numpunct<Pt::Char> >( std::locale() );

    // Reserve some bytes within the result buffer
    resultBuffer.reserve(512);

    // Variables for processing argument(s)
    bool         gotArgFld = false;

    std::string  argIdxStr;
    unsigned int argIdxMan = 0;
    unsigned int argIdxCnt = 0;
    unsigned int argIdxEff;

    bool         gotColon  = false;
    std::string  numberStr;

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
                argIdxEff = parseUInt(argIdxStr.c_str());
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
                fsSpec.width = parseUInt(numberStr.c_str());
            }
            // Check if the next character is '.'
            CHECK_FOR_CLOSING_BRACKET();
            if(*it == '.') {
                // Read the 'precision'
                ++it;
                numberStr.clear();
                while( isdigit(*it) ) numberStr += *it++;
                if(!numberStr.empty()) {
                    fsSpec.precision = parseUInt(numberStr.c_str());
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
