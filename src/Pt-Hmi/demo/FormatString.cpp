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
// https://en.cppreference.com/w/cpp/utility/format/format
// https://en.cppreference.com/w/cpp/utility/format/formatter#Standard_format_specification
// https://en.cppreference.com/w/cpp/chrono/system_clock/formatter#Format_specification
// https://en.cppreference.com/w/cpp/locale/locale
// https://en.cppreference.com/w/cpp/locale/num_put
// https://en.cppreference.com/w/cpp/locale/numpunct
//
void FormatStringArg::ff_I8(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_I8 Not implemented yet!");
}


void FormatStringArg::ff_U8(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_U8 Not implemented yet!");
}


void FormatStringArg::ff_I16(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_I16 Not implemented yet!");
}


void FormatStringArg::ff_U16(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_U16 Not implemented yet!");
}


void FormatStringArg::ff_I32(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_I32 Not implemented yet!");
}


void FormatStringArg::ff_U32(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_U32 Not implemented yet!");
}


void FormatStringArg::ff_I64(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_I64 Not implemented yet!");
}


void FormatStringArg::ff_U64(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_U64 Not implemented yet!");
}


void FormatStringArg::ff_F(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_F Not implemented yet!");
}


void FormatStringArg::ff_D(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_D Not implemented yet!");
}


void FormatStringArg::ff_LD(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_LD Not implemented yet!");
}


void FormatStringArg::ff_B(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_B Not implemented yet!");
}


void FormatStringArg::ff_P(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    throw FormatStringError("ff_P Not implemented yet!");
}


void FormatStringArg::ff_S(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
{
    // Check the type
    if(fss.type && fss.type != 's')
        throw FormatStringError("invalid 'type' in format string");

    // Get the alignment
    const char align = fss.align ? fss.align : '<'; // The default alignment for string is left

    // Get the length of the string
    const size_t strLen = _valStr.length();

    // Center
    if(align == '^') {
        // Calculate the padding
        const size_t dlen = fss.width - strLen;
        const size_t lpad = dlen / 2;
        const size_t rpad = (lpad * 2 == dlen) ? lpad : (lpad + 1);
        // Left pad the string as needed
        for(size_t i = 0; i < lpad; ++i) rbf += fss.fill;
        // Put the string
        rbf += _valStr;
        // Right pad the string as needed
        for(size_t i = 0; i < rpad; ++i) rbf += fss.fill;
    }
    // Right
    else if(align == '>') {
        // Left pad the string as needed
        if(strLen < fss.width) {
            for(size_t i = 0; i < (fss.width - strLen); ++i) rbf += fss.fill;
        }
        // Put the string
        rbf += _valStr;
    }
    // Left
    else if(align == '<') {
        // Put the string
        rbf += _valStr;
        // Right pad the string as needed
        if(strLen < fss.width) {
            for(size_t i = 0; i < (fss.width - strLen); ++i) rbf += fss.fill;
        }
    }
    // Invalid
    else {
        throw FormatStringError("invalid 'align' in format string");
    }

/*
    // fill-and-align(optional) sign(optional) #(optional) 0(optional) width(optional) precision(optional) L(optional) type(optional)
    char sign;       // + - [space]
    bool altForm;    // #
    bool zeroPad;    // 0
    int  precision;  // floating-point precision (default 6)
    bool locale;     // use locale-specific formatting
*/
}


//
// The main string-formatter class
//
const void FormatString::operator()(Pt::String& resultBuffer) const
{
    // Format-string specifier
    FormatStringSpec fsSpec;

    // Get the "numpunct" instance
    const std::numpunct<Pt::Char>& numpunct = std::use_facet< std::numpunct<Pt::Char> >( std::locale() );

    // Variable for processing argument(s)
    std::string  argIdxStr;
    unsigned int argIdxMan = 0;
    unsigned int argIdxCnt = 0;
    unsigned int argIdxEff;

    bool         gotArgFld = false;

    // Walk through the characters of the format string
    //Pt::String::const_iterator it    = _format.begin();
    //Pt::String::const_iterator itEnd = _format.end();
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
                throw FormatStringError("argument index out of range");
            }
            // Process (format) the argument
            const FormatStringArg& arg = *( (*_args)[argIdxEff] );
            arg(resultBuffer, fsSpec, numpunct);
            // Clear the flags
            gotArgFld = false;
            fsSpec.reset();
            // Break loop if all the format characters have been processed
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
            // Read the argument index
            argIdxStr.clear();
            while( isdigit(*it) ) argIdxStr += *it++;
            // Check if the next character is '}'
            if(*it == '}') {
                ++it;
                gotArgFld = true;
                continue;
            }
            // {0:*<-#08.6Ls}
            //   *

/*
    // fill-and-align(optional) sign(optional) #(optional) 0(optional) width(optional) precision(optional) L(optional) type(optional)
    char fill;       // fill character
    char align;      // < > ^
    char sign;       // + - [space]
    bool altForm;    // #
    bool zeroPad;    // 0
    int  width;      // minimum field width (default 0)
    int  precision;  // floating-point precision (default 6)
    bool locale;     // use locale-specific formatting
    char type;       // none/s b B c d o x X a A e E f/F g G p
*/
            // Error
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
