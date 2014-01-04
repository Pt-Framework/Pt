/*
 * Copyright (C) 2004-2006 Marc Boris Duerner
 * Copyright (C)      2006 Aloysius Indrayanto
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

#ifndef Pt_Byteorder_h
#define Pt_Byteorder_h

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <cassert>

// build systems can specify byte-order by defining PT_LE or PT_BE.
// If these are not defined it is still possible to detect the
// endianess correctly on many common targets.
#if defined(PT_LE) || defined(PT_BE)
    // user defined endianess
#elif defined (_BYTE_ORDER)
#   if (_BYTE_ORDER == _LITTLE_ENDIAN)
#       define PT_LE
#   elif (_BYTE_ORDER == _BIG_ENDIAN)
#       define PT_BE
#   else
#       error: unknown _BYTE_ORDER
#   endif
#elif defined (__BYTE_ORDER)
#    if (__BYTE_ORDER == __LITTLE_ENDIAN)
#        define PT_LE
#    elif (__BYTE_ORDER == __BIG_ENDIAN)
#        define PT_BE
#    else
#        error: unknown __BYTE_ORDER
#    endif
#elif defined (__LITTLE_ENDIAN__) || \
      defined (i386) || defined(__i386) || defined (__i386__) || \
      defined(_X86_) || defined(sun386) || defined (_M_IX86) ||  \
      defined (_M_IA64) || defined (__ia64__) || \
      defined(__IA64__) || defined(_IA64) || \
      defined (_M_AMD64) || defined (__amd64) || \
      defined(MIPSEL) || defined(_MIPSEL) || \
      defined (ARM) || defined(__arm__) || defined(_M_ARM) || defined(_M_ARMT) || \
      defined (vax) || defined (__alpha) || defined(__THW_INTEL)
    #define PT_LE
#elif defined(__BIG_ENDIAN__) || \
      defined(__hppa__) || defined(__hppa) || defined(__hp9000) || \
      defined(__hp9000s300) || defined(hp9000s300) || \
      defined(__hp9000s700) || defined(hp9000s700) || \
      defined(__hp9000s800) || defined(hp9000s800) || defined(hp9000s820) || \
      defined(__sparc__) || defined(sparc) || defined(__sparc) || \
      defined(ibm032) || defined(ibm370) || defined(_IBMR2) || \
      defined(MIPSEB) || defined(_MIPSEB) || \
      defined(mc68000) || defined(is68k) || defined(macII) || defined(m68k) || \
      defined(apollo) || defined(__convex__) || defined(_CRAY) || defined(sel)
    #define PT_BE
#else
    #error: PT_LE or PT_BE is not defined
#endif

namespace Pt {

    /** @internal Swaps the byteorder of the given 16-bit value.
    */
    template <typename T>
    inline T swab16(T value)
    {
        //union {
        //    uint16_t v;
        //    uint8_t  b[2];
        //} u;
        //u.v = value;
        //const uint8_t b0 = u.b[0];
        //const uint8_t b1 = u.b[1];
        //u.b[0] = b1;
        //u.b[1] = b0;
        //return(u.v);

        return ( (value & 0x00FF) << 8 ) |
               ( (value & 0xFF00) >> 8 );
    }

    /** @internal Swaps the byteorder of the given 32-bit value.
    */
    template <typename T>
    inline T swab32(T value)
    {
        return ( (value & 0x000000FF) << 24 ) |
               ( (value & 0x0000FF00) <<  8 ) |
               ( (value & 0x00FF0000) >>  8 ) |
               ( (value & 0xFF000000) >> 24 );
    }

#ifdef PT_WITH_INT64
    /** @internal Swaps the byteorder of the given 64-bit value.
    */
    template <typename T>
    inline T swab64(T value)
    {
        return ( (value & 0x00000000000000FFULL) << 56 ) |
               ( (value & 0x000000000000FF00ULL) << 40 ) |
               ( (value & 0x0000000000FF0000ULL) << 24 ) |
               ( (value & 0x00000000FF000000ULL) <<  8 ) |
               ( (value & 0x000000FF00000000ULL) >>  8 ) |
               ( (value & 0x0000FF0000000000ULL) >> 24 ) |
               ( (value & 0x00FF000000000000ULL) >> 40 ) |
               ( (value & 0xFF00000000000000ULL) >> 56 );
    }
#endif

    /** @brief Stub function for byteorder conversion.

        @ingroup CoreTypes
    */
    inline int8_t swab(int8_t value)
    { return value; }

    /** @brief Stub function for byteorder conversiong.

        @ingroup CoreTypes
    */
    inline uint8_t swab(uint8_t value)
    { return value; }

    /** @brief Swaps the byteorder of an int16_t.
        
        @ingroup CoreTypes
     */
    inline int16_t swab(int16_t value)
    { return swab16(value); }

    /** @brief Swaps the byteorder of a uint16_t.
        
        @ingroup CoreTypes
     */
    inline uint16_t swab(uint16_t value)
    { return swab16(value); }

    /** @brief Swaps the byteorder of an int32_t.
        
        @ingroup CoreTypes
     */
    inline int32_t swab(int32_t value)
    { return swab32(value); }

    /** @brief Swaps the byteorder of a uint32_t.
        
        @ingroup CoreTypes
    */
    inline uint32_t swab(uint32_t value)
    { return swab32(value); }

#ifdef PT_WITH_INT64
    /** @brief Swaps the byteorder of an int64_t.
        
        @ingroup CoreTypes
    */
    inline int64_t swab(int64_t value)
    { return swab64(value); }

    /** @brief Swaps the byteorder of a uint64t.
        
        @ingroup CoreTypes
    */
    inline uint64_t swab(uint64_t value)
    { return swab64(value); }
#endif

    //! @brief Returns true, if the cpu is big-endian (high-byte first).
    inline bool isBigEndian()
    {
        const int i = 1;
        return *reinterpret_cast<const int8_t*>(&i) == 0;
    }

    //! @brief Returns true, if the cpu is little-endian (low-byte first).
    inline bool isLittleEndian()
    {
        const int i = 1;
        return *reinterpret_cast<const int8_t*>(&i) == 1;
    }

    /** @brief Converts a value from host-byteorder to little-endian.
     
        This function does nothing on a LE system, but calls swap() on a BE system.
        The generic swap() function expects the type passed in to be an integer type
        and so does this function. Overloading swab can remove this restriction and
        may improve performance for custom types.
       
        @ingroup CoreTypes
     */
    template <typename T>
    inline T hostToLe(const T& value)
    {
#ifdef PT_LE
        return value;
#else
        return swab(value);
#endif
    }

    /** @brief Converts a value from little-endian to host-byteorder.

        This function does nothing on a LE system, but calls swap() on a BE system.
        The generic swap() function expects the type passed in to be an integer type
        and so does this function. Overloading swab can remove this restriction and
        may improve performance for custom types.

        @ingroup CoreTypes
     */
    template <typename T>
    inline T leToHost(const T& value)
    {
#ifdef PT_LE
        return value;
#else
        return swab(value);
#endif
    }

    /** @brief Converts a value from the host-byteorder to big-endian.

        This function does nothing on a BE system, but calls swap() on a LE system.
        The generic swap() function expects the type passed in to be an integer type
        and so does this function. Overloading swab can remove this restriction and
        may improve performance for custom types.
       
        @ingroup CoreTypes
     */
    template <typename T>
    inline T hostToBe(const T& value)
    {
#ifdef PT_LE
        return swab(value);
#else
        return value;
#endif
    }

    /** @brief Converts a value from big-endian to host-byteorder.
     
        This function does nothing on a BE system, but calls swap() on a LE system.
        The generic swap() function expects the type passed in to be an integer type
        and so does this function. Overloading swab can remove this restriction and
        may improve performance for custom types.

        @ingroup CoreTypes
     */
    template <typename T>
    inline T beToHost(const T& value)
    {
#ifdef PT_LE
        return swab(value);
#else
        return value;
#endif
    }

} // namespace Pt

#endif
