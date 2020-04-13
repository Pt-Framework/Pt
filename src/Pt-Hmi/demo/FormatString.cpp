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
    throw FormatStringError("ff_S Not implemented yet!");
}


//
// The main string-formatter class
//
const Pt::String FormatString::operator()() const
{
    // Initialize the variables
    Pt::String                     resBuff;
    FormatStringSpec               fsSpec;
    const std::numpunct<Pt::Char>& numpunct = std::use_facet< std::numpunct<Pt::Char> >( std::locale() );

    // Walk through the characters
    Pt::String::const_iterator it    = _format.begin();
    Pt::String::const_iterator itEnd = _format.end();

    // {0:*<-#08.6Ls}

//    TEST_AND_BENCHMARK("|{0:8}| |{0:*<8}| |{0:*>8}| |{0:*^8}| |{0:^8}|", "aBc");

    //while(it != itEnd) {
    //}
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

    //(*_args[0])(resBuff, FormatStringSpec(), numpunct);

    return resBuff;
}
/*
  what():  missing '}' in format string
  what():  unmatched '}' in format string
  what():  cannot switch from automatic to manual argument indexing
  what():  cannot switch from manual to automatic argument indexing
  what():  argument index out of range
 */

} // namespace
