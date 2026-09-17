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

#ifndef PT_API_INTTYPES_H
#define PT_API_INTTYPES_H

/** @defgroup Pt-IntTypes Fixed-Size Integers

    @brief Fixed-size, platform-independent signed and unsigned integer types.

    The %Pt framework defines a number of fixed-size, signed and unsigned
    integers ranging from 8-bit to 64-bit widths. They are typedefs for
    builtin fundamental types such as int or long and the actual type
    depends on the platform. For example, Pt::uint8_t is a typedef for an
    unsigned 8 bits wide integer type and Pt::int32_t is a typedef for a
    signed 32 bits wide integer. The following table shows all available
    fixed-size integer types:

    <table style="width:600px; margin:20px; padding:6px; background-color: #eff0f0;">
        <tr>
            <td><b>int8_t</b></td>
            <td>signed 8 bit integer</td>
            <td><b>uint8_t</b></td>
            <td>unsigned 8 bit integer</td>
        </tr>
        <tr>
            <td><b>int16_t</b></td>
            <td>signed 16 bit integer</td>
            <td><b>uint16_t</b></td>
            <td>unsigned 16 bit integer</td>
        </tr>
        <tr>
            <td><b>int32_t</b></td>
            <td>signed 32 bit integer</td>
            <td><b>uint32_t</b></td>
            <td>unsigned 32 bit integer</td>
        </tr>
        <tr>
            <td><b>int64_t</b></td>
            <td>signed 64 bit integer</td>
            <td><b>uint64_t</b></td>
            <td>unsigned 64 bit integer</td>
        </tr>
    </table>

    @ingroup Pt-Basics
*/

#endif
