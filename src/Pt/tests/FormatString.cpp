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

static inline unsigned int parseUInt(const char *p)
{
    unsigned int v = 0;

    while(*p != '\0') {
        v = (v * 10) + (*p - '0');
        ++p;
    }

    return v;
}


/*
// A functor that doesn't add a thousands separator.
struct NoThousandsSep {
  template <typename Char>
  void operator()(Char *) {}
};


// A functor that adds a thousands separator.
class ThousandsSep {
 private:
  fmt::StringRef sep_;

  // Index of a decimal digit with the least significant digit having index 0.
  unsigned digit_index_;

 public:
  explicit ThousandsSep(fmt::StringRef sep) : sep_(sep), digit_index_(0) {}

  template <typename Char>
  void operator()(Char *&buffer) {
    if (++digit_index_ % 3 != 0)
      return;
    buffer -= sep_.size();
    std::uninitialized_copy(sep_.data(), sep_.data() + sep_.size(),
                            internal::make_ptr(buffer, sep_.size()));
  }
};
*/
/*
// {fmt} - Victor Zverovich (vitaut) and Jonathan Müller - MIT license
// https://github.com/fmtlib/fmt/releases/tag/4.1.0
static const char DIGITS[] =
    "0001020304050607080910111213141516171819"
    "2021222324252627282930313233343536373839"
    "4041424344454647484950515253545556575859"
    "6061626364656667686970717273747576777879"
    "8081828384858687888990919293949596979899";

template <typename ValueT, typename ThousandsSep>
inline void uintToString(Pt::String& dst, ValueT val

template <typename UInt, typename Char, typename ThousandsSep>
inline void format_decimal(Char *buffer, UInt value, unsigned num_digits,
                           ThousandsSep thousands_sep) {
  buffer += num_digits;
  while (value >= 100) {
    // Integer division is slow so do it for a group of two digits instead
    // of for every digit. The idea comes from the talk by Alexandrescu
    // "Three Optimization Tips for C++". See speed-test for a comparison.
    unsigned index = static_cast<unsigned>((value % 100) * 2);
    value /= 100;
    *--buffer = Data::DIGITS[index + 1];
    thousands_sep(buffer);
    *--buffer = Data::DIGITS[index];
    thousands_sep(buffer);
  }
  if (value < 10) {
    *--buffer = static_cast<char>('0' + value);
    return;
  }
  unsigned index = static_cast<unsigned>(value * 2);
  *--buffer = Data::DIGITS[index + 1];
  thousands_sep(buffer);
  *--buffer = Data::DIGITS[index];
}
*/

template <typename T>
static inline void printUIntRev(Pt::String& dst, T val, T base)
{
    static const char* DIGITS = "0123456789abcdef";

    do {
        dst += DIGITS[val % base];
        val /= base;
    } while(val != 0);
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
void FormatStringArg::ff_I8(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    throw FormatStringError("ff_I8 is not implemented yet!");
}


void FormatStringArg::ff_U8(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    throw FormatStringError("ff_U8 is not implemented yet!");
}


void FormatStringArg::ff_I16(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    throw FormatStringError("ff_I16 is not implemented yet!");
}


void FormatStringArg::ff_U16(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    throw FormatStringError("ff_U16 is not implemented yet!");
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
void FormatStringArg::ff_I32(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    // Process as base 2 type
    if( TYPE_IS_B(fss.type) ) {
        throw FormatStringError("ff_I32 'b' is not implemented yet!");
    }
    // Process as base 8 type
    else if( TYPE_IS_O(fss.type) ) {
        throw FormatStringError("ff_I32 'o' is not implemented yet!");
    }
    // Process as base 16 type
    else if( TYPE_IS_X(fss.type) ) {
        throw FormatStringError("ff_I32 'x' is not implemented yet!");
    /*
         # For integral types, when binary, octal, or hexadecimal presentation type is used, the alternate form
           inserts the prefix (0b, 0, or 0x) into the output value after the sign character (possibly space) if
           there is one, or add it before the output value otherwise.
     */
    }
    // Process as base 10 type
    else if( TYPE_IS_D(fss.type) ) {
        // Convert to string (in reserved direction)
        Pt::String str;
        printUIntRev(str, _valPOD.i32, 10);
        _valStr = str;
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

        //
        if(fss.sign) {
            fss.sign = 0;
        }
        //
        if(fss.altForm) {
            fss.altForm = false;
        }
        //
        if(fss.zeroPad) {
            fss.zeroPad = false;
        }
    }
    // Invalid type
    else {
        throw FormatStringError("invalid 'type specifier' in format string");
    }

    // Process the generated string
    fss.type   = 's';
    fss.locale = false;
    ff_S(rbf, fss, numpunct);
}


void FormatStringArg::ff_U32(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    throw FormatStringError("ff_U32 is not implemented yet!");
}


void FormatStringArg::ff_I64(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    throw FormatStringError("ff_I64 is not implemented yet!");
}


void FormatStringArg::ff_U64(Pt::String &rbf, FormatStringSpec& fss, const numpunct_t* numpunct) const
{
    throw FormatStringError("ff_U64 is not implemented yet!");
}


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

    //if(fss.zeroPad)
    //    throw FormatStringError("format specifier '0' requires numeric argument");

    // Process as string type
    if( TYPE_IS_S(fss.type) ) {
        //
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
        _valPOD.u8 = _valPOD.b ? 1 : 0;
        fss.type   = 'd'; // For boolean type 'c' is assumed as 'd'
        ff_U8(rbf, fss, numpunct);
    }
    else if( TYPE_IS_BDOX(fss.type) ) {
        _valPOD.u8 = _valPOD.b ? 1 : 0;
        ff_U8(rbf, fss, numpunct);
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
        _valPOD.u8 = _valPOD.b ? 1 : 0;
        ff_U8(rbf, fss, numpunct);
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
