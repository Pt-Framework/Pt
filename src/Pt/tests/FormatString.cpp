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

#include "FormatString.h"


namespace Pt {


//
// Utility functions
//
static inline size_t parseSizeT(const char *p)
{
    size_t v = 0;

    while(*p != '\0') {
        v = (v * 10) + (*p - '0');
        ++p;
    }

    return v;
}


//
// The string-formatter class
//
const void FormatString::operator()(Pt::String& resultBuffer, const FormatStringValue::locale_t* customLocale) const
{
    // TODO: Optimize!

#define CHECK_FOR_CLOSING_BRACKET() \
    if(*it == '}') {                \
        ++it;                       \
        gotArgFld = true;           \
        continue;                   \
    }                               \
    do {} while(false)              \

    // Formatting rule
    FormatStringValue::Rule rule;

    // Get the "numpunct" instance (if supported)
#ifdef PT_WITH_STD_LOCALE
    const FormatStringValue::numpunct_t* numpunct =
        customLocale ? &std::use_facet<FormatStringValue::numpunct_t>( *customLocale )
                     : &std::use_facet<FormatStringValue::numpunct_t>( std::locale() );
    //if(numpunct && numpunct->grouping().length() > 1)
    //    throw FormatStringError("locale with complex digit grouping is not supported");
#else
    const FormatStringValue::numpunct_t* numpunct = 0;
#endif

    // Reserve some bytes within the result buffer
    resultBuffer.reserve(256);

    // Variables for processing argument(s)
    bool        gotArgFld = false;

    std::string argIdxStr;
    size_t      argIdxMan = 0;
    size_t      argIdxCnt = 0;
    size_t      argIdxEff;

    bool        gotColon  = false;
    std::string numberStr;

    // Walk through the format characters
    //
    // fill-and-align(optional) sign(optional) #(optional) 0(optional) width(optional) precision(optional) L(optional) type(optional)
    //
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
            const FormatStringValue& arg = *( (*_args)[argIdxEff] );
            arg(resultBuffer, rule, numpunct);
            // Clear the flags
            gotArgFld = false;
            rule.reset();
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
                rule.setAlign( (FormatStringValue::Rule::Align) it->value() );
                ++it;
            }
            else if(*(it + 1) == '<' || *(it + 1) == '>' || *(it + 1) == '^') {
                rule.setFill (*it++);
                rule.setAlign( (FormatStringValue::Rule::Align) it->value() );
                ++it;
            }
            // Read the 'sign'
            CHECK_FOR_CLOSING_BRACKET();
            if(*it == '+' || *it == '-' || *it == ' ') {
                rule.setSign(*it++);
            }
            // Read the '#'
            CHECK_FOR_CLOSING_BRACKET();
            if(*it == '#') {
                ++it;
                rule.setAltForm(true);
            }
            // Read the '0'
            CHECK_FOR_CLOSING_BRACKET();
            if(*it == '0') {
                ++it;
                rule.setZeroPad(true);
            }
            // Read the 'width'
            CHECK_FOR_CLOSING_BRACKET();
            numberStr.clear();
            while( isdigit(*it) ) numberStr += *it++;
            if(!numberStr.empty()) {
                rule.setWidth( parseSizeT(numberStr.c_str()) );
            }
            // Check if the next character is '.'
            CHECK_FOR_CLOSING_BRACKET();
            if(*it == '.') {
                // Read the 'precision'
                ++it;
                numberStr.clear();
                while( isdigit(*it) ) numberStr += *it++;
                if(!numberStr.empty()) {
                    rule.setPrecision( parseSizeT(numberStr.c_str()) );
                }
                else {
                    throw FormatStringError("missing 'precision specifier' in format string");
                }
            }
            // Read the 'locale'
            CHECK_FOR_CLOSING_BRACKET();
            if(*it == 'L') {
                ++it;
                rule.setLocale(true);
            }
            // Read the 'type'
            CHECK_FOR_CLOSING_BRACKET();
            rule.setType(*it++);
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

