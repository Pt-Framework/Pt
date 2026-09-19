/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_API_BYTEORDER_H
#define PT_API_BYTEORDER_H

/** @addtogroup Pt-Byteorder

    @brief Little-endian and big-endian integer conversion.

    A multi-byte integer has a byte order. On a little-endian host the
    least significant byte is stored first; on a big-endian host the
    most significant byte is stored first. File formats and network
    protocols pick one of those orders. This group converts between
    that external order and the order of the CPU that runs the
    program.

    The host order is a compile-time fact. The headers define @c PT_LE
    or @c PT_BE from the toolchain, or from an explicit build setting.
    %isLittleEndian() and %isBigEndian() answer the same question at
    run time by inspecting a known integer in memory. Prefer the
    compile-time macros when the conversion can vanish on a matching
    host. Use the run-time queries when the answer must be a value.

    %swab() reverses the bytes of a fixed-size integer from
    @ref Pt-IntTypes. An 8-bit value is unchanged. 16-bit, 32-bit, and
    64-bit values swap every byte. %swab() always swaps; it does not
    know about host order. Overload %swab() for a type that is not an
    integer when that type has a defined byte-wise reverse.

    %hostToLe() and %leToHost() convert between host order and
    little-endian. On a little-endian host they return the value
    unchanged. On a big-endian host they call %swab(). %hostToBe() and
    %beToHost() do the same for big-endian: they are no-ops on a
    big-endian host and swap on a little-endian host. Passing a value
    that is already in host order through %hostToLe() produces the
    little-endian layout to store in a file or on the wire. Passing a
    little-endian value from a file through %leToHost() produces a
    host integer that arithmetic can use.

    @code
    Pt::uint32_t host = 0x01020304;
    Pt::uint32_t le = Pt::hostToLe(host);
    Pt::uint32_t back = Pt::leToHost(le);
    @endcode

    The conversions are templates on the integer type. They expect a
    type that %swab() already handles, or a type with its own %swab()
    overload. Mixing a host integer with an external integer without
    these functions is the usual mistake: the value looks right on one
    endianness and silently wrong on the other.
*/

#endif
