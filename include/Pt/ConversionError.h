/*
 * Copyright (C) 2004-2007 by Marc Boris Duerner
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

#ifndef PT_CONVERSIONERROR_H
#define PT_CONVERSIONERROR_H

#include <Pt/Api.h>
#include <stdexcept>

namespace Pt {

/** @brief Indicates a failed conversion.

    This exception is thrown, when conversions between types fail. For
    example, converting a string to an integer can fail if the string does
    not represent a number. 
*/
class PT_API ConversionError : public std::runtime_error
{
    public:
        //! @brief Construct with error message.
        explicit ConversionError(const std::string& msg);

        //! @brief Construct with error message.
        explicit ConversionError(const char* msg);

        //! @brief Destructor.
        ~ConversionError() throw()
        {}
};

} // namespace Pt

#endif
