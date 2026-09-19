/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_API_INTTYPES_H
#define PT_API_INTTYPES_H

/** @addtogroup Pt-IntTypes

    @brief Fixed-width signed and unsigned integer types.

    A width that is part of a contract belongs in a type, not in a comment.
    File formats, network protocols, atomic cells, and byte-order conversion
    all name an exact number of bits. The C++ fundamental types do not:
    @c int may be 16 or 32 bits, @c long may be 32 or 64. This group is the
    portable spelling of those widths.

    Each name is a typedef for a fundamental type that has that width on the
    current platform. %Pt::int32_t is a signed 32-bit integer; %Pt::uint8_t
    is an unsigned 8-bit integer. The typedef may be @c int on one target
    and @c long on another. Code that uses the %Pt name keeps the width
    without writing per-platform #if.

    The signed and unsigned pairs are:

    <table style="width:600px; margin:20px; padding:6px; background-color: #eff0f0;">
        <tr>
            <td><b>int8_t</b></td>
            <td>signed 8-bit integer</td>
            <td><b>uint8_t</b></td>
            <td>unsigned 8-bit integer</td>
        </tr>
        <tr>
            <td><b>int16_t</b></td>
            <td>signed 16-bit integer</td>
            <td><b>uint16_t</b></td>
            <td>unsigned 16-bit integer</td>
        </tr>
        <tr>
            <td><b>int32_t</b></td>
            <td>signed 32-bit integer</td>
            <td><b>uint32_t</b></td>
            <td>unsigned 32-bit integer</td>
        </tr>
        <tr>
            <td><b>int64_t</b></td>
            <td>signed 64-bit integer</td>
            <td><b>uint64_t</b></td>
            <td>unsigned 64-bit integer</td>
        </tr>
    </table>

    Use these types where the width is the contract. Use @c int or
    @c std::size_t where the platform's natural width is the contract.
    Byte-order conversion in @ref Pt-Byteorder and the atomic cell in
    @ref Pt-Atomics are defined in terms of these widths.
*/

#endif
