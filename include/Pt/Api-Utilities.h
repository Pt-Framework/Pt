/* Copyright (C) 2008 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_API_UTILITIES_H
#define PT_API_UTILITIES_H

/** @defgroup Utilities Utilities

    @brief Conversion utilities, program options and type information.

    The conversion utilities of the Platinum core library include functions
    to convert @link utilities_ByteOrder byte orders@endlink, to format and
    parse @link utilities_StringConv numbers@endlink, and to perform checked
    @link utilities_NumConv numeric conversions@endlink.

    Basic application support is provided by the Pt::Arg class and the
    Pt::Settings class. The first one is a convenient way to parse and 
    process program options and the latter one allows to load and store
    application settings in files or other places.

    Two types are useful to get type information Pt::TypeTraits and
    Pt::TypeInfo. TypeTraits are used for generic programming, for example
    to deduce the pointer type in templated code, or to branch differently
    for const and non-const types. The TypeInfo class is a wrapper for
    std::type_info, which makes it easier to store and compare type information.
    The std::type_info is normally not copyable and comparable. Pt::SourceInfo
    is used to store information about a location in the source code.
*/

#endif
