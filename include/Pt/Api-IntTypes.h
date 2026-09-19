/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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
