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

#include <Pt/StringStream.h>

#include "FormatString.h"


namespace Pt {


//
// Format error exception
//
FormatError::FormatError(const std::string& msg)
: std::runtime_error(msg)
{
}


FormatError::FormatError(const char* msg)
: std::runtime_error(msg)
{
}


// https://en.cppreference.com/w/cpp/utility/format/format
// https://en.cppreference.com/w/cpp/utility/format/formatter#Standard_format_specification
// https://en.cppreference.com/w/cpp/chrono/system_clock/formatter#Format_specification
// https://en.cppreference.com/w/cpp/locale/locale
// https://en.cppreference.com/w/cpp/locale/num_put
// https://en.cppreference.com/w/cpp/locale/numpunct

// svn commit -m 'Trying to implement a simple string formatter ala std::format that uses Pt::String'


//
// Argument wrapper & formatter
//

const Pt::String FormatArg::operator()(const FormatSpec& fs) const
{
    return "";
}


//
// The main string formatter
//
const Pt::String FormatString::operator()() const
{
    const std::num_put <Pt::Char>& numput   = std::use_facet< std::num_put <Pt::Char> >( std::locale() );
    const std::numpunct<Pt::Char>& numpunct = std::use_facet< std::numpunct<Pt::Char> >( std::locale() );

    Pt::OStringStream oss;

    numput.put(oss, oss, '0', 2.71);
    oss << " " << numpunct.truename();
    oss << "\n";

    return oss.str();
}


}
