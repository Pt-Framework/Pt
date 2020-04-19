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

#ifndef PT_FORMAT_STRING_H
#define PT_FORMAT_STRING_H

#include <vector>

#include "FormatStringValue.h"


namespace Pt {


//
// The string-formatter class
//
class PT_API FormatString {
    public:
        // Constructor
        inline FormatString(const Pt::String& format, const std::vector<const FormatStringValue*>* args)
        : _format(format), _args(args)
        {}

        // Format the value(s) using the already specified format and optional locale
        const void operator()(Pt::String& resultBuffer, const FormatStringValue::locale_t* customLocale = 0) const;

    private:
        const Pt::String                             _format;
        const std::vector<const FormatStringValue*>* _args;
};


} // namespace


//
// Include the front-ends functions
//
#include "FormatString_FrontEnd.h"


#endif

