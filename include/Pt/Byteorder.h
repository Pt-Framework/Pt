/***************************************************************************
 *   Copyright (C) 2004-2006 Marc Boris Duerner                            *
 *   Copyright (C)      2006 Aloysius Indrayanto                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_Byteorder_h
#define Pt_Byteorder_h

#include <cassert>

#include <Pt/Api.h>
#include <Pt/Types.h>


// Check if the proper macro is defined or not
#if !defined(PT_LE) && !defined(PT_BE)
#error "PT_LE or PT_BE needs to be defined."
#endif

// For experiment
#define USE_BYTE_MOVE

namespace Pt
{
	/** @brief Swaps the byteorder of the given 16-bit value.
		* @internal
		*/
	template <typename T>
	inline T swab16(T value)
	{
#ifdef USE_BYTE_MOVE
		union {
			uint16_t v;
			uint8_t  b[2];
		} u;
		u.v = value;
		const uint8_t b0 = u.b[0];
		const uint8_t b1 = u.b[1];
		u.b[0] = b1;
		u.b[1] = b0;
		return(u.v);
	/*
		movl	8(%ebp), %edx
		movl	%edx, %eax
		movzbl	%dh, %ecx
		movb	%cl, %al
		movb	%dl, %ah
		movzwl	%ax, %eax
		leave
		ret
	*/
#else
		return ( (value & 0x00FF) << 8 ) |
					 ( (value & 0xFF00) >> 8 );
	/*
		movzwl	8(%ebp), %edx
		movl	%edx, %eax
		sall	$8, %eax
		shrl	$8, %edx
		orl	%edx, %eax
		movzwl	%ax, %eax
		leave
		ret
	*/
#endif
	}

	/** @brief Swaps the byteorder of the given 32-bit value.
	 *  @internal
	 */
	template <typename T>
	inline T swab32(T value)
	{
#ifdef USE_BYTE_MOVE
		uint8_t *w = reinterpret_cast<uint8_t*>(&value);

		const uint8_t w0 = w[0];
		const uint8_t w1 = w[1];
		const uint8_t w2 = w[2];
		const uint8_t w3 = w[3];
		w[0] = w3;
		w[1] = w2;
		w[2] = w1;
		w[3] = w0;
		return(value);
	/*
		movb	8(%ebp), %cl
		movb	9(%ebp), %dl
		movb	11(%ebp), %al
		movb	%al, 8(%ebp)
		movb	10(%ebp), %al
		movb	%al, 9(%ebp)
		movb	%dl, 10(%ebp)
		movb	%cl, 11(%ebp)
		movl	8(%ebp), %eax
		leave
		ret
	*/
#else
		return ( (value & 0x000000FF) << 24 ) |
						 ( (value & 0x0000FF00) <<  8 ) |
						 ( (value & 0x00FF0000) >>  8 ) |
						 ( (value & 0xFF000000) >> 24 );
	/*
		movl	8(%ebp), %ecx
		movl	%ecx, %eax
		sall	$24, %eax
		movl	%ecx, %edx
		andl	$65280, %edx
		sall	$8, %edx
		orl	%edx, %eax
		movl	%ecx, %edx
		andl	$16711680, %edx
		shrl	$8, %edx
		shrl	$24, %ecx
		orl	%ecx, %edx
		orl	%edx, %eax
		leave
		ret
	*/
#endif
	}

	#ifdef PT_64BIT
	/** @brief Swaps the byteorder of the given 64-bit value.
	 *  @internal
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




	/** @brief Dummy function which does nothing.
	 *
	 *  Just for the sake of completeness.
	 */
	inline int8_t swap(int8_t value)
	{ return value; }

	/** @brief Dummy function which does nothing.
	 *
	 *  Just for the sake of completeness.
	 */
	inline uint8_t swap(uint8_t value)
	{ return value; }


	/** @brief Swaps the byteorder of an int16_t.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline int16_t swap(int16_t value)
	{ return swab16(value); }

	/** @brief Swaps the byteorder of a uint16_t.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline uint16_t swap(uint16_t value)
	{ return swab16(value); }


	/** @brief Swaps the byteorder of an int32_t.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline int32_t swap(int32_t value)
	{ return swab32(value); }

	/** @brief Swaps the byteorder of a uint32_t.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline uint32_t swap(uint32_t value)
	{ return swab32(value); }


#ifdef PT_64BIT
	/** @brief Swaps the byteorder of an int64_t.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline int64_t swap(int64_t value)
	{ return swab64(value); }

	/** @brief Swaps the byteorder of a uint64t.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline uint64_t swap(uint64_t value)
	{ return swab64(value); }
#endif




	/** @brief Changes the byteorder of a given value from host-byteorder to little-endian.
	 *
	 *  @param value The value in host-byteorder
	 *  @return The value changed to little-endian
	 *
	 *  This function does nothing on a LE system, but calls swap() on a BE system.
	 *  The generic swap() function expects the type passed in to be bitwise-copyable
	 *  and thus does this function. Overloading swab can remove this restriction and
	 *  may improve performance for custon types.
	 */
	template <typename T>
	inline T hostToLe(const T& value)
	{
#ifdef PT_LE
		return value;
#else
		return swap(value);
#endif
	}

	/** @brief Changes the byteorder of a given value from little-endian to host-byteorder.
	 *
	 *  @param value The value in host-byteorder
	 *  @return The value changed to little-endian
	 *
	 *  This function does nothing on a LE system, but calls swap() on a BE system.
	 *  The generic swap() function expects the type passed in to be bitwise-copyable
	 *  and thus does this function. Overloading swab can remove this restriction and
	 *  may improve performance for custon types.
	 */
	template <typename T>
	inline T leToHost(const T& value)
	{
#ifdef PT_LE
		return value;
#else
		return swap(value);
#endif
	}


	/** @brief Changes the given value from the host-byteorder to big-endian.
	 *
	 *  @param value The value in host-byteorder
	 *  @return The value in big-endian
	 *
	 *  This function does nothing on a BE system, but calls swap() on a LE system.
	 *  The generic swap() function expects the type passed in to be bitwise-copyable
	 *  and thus does this function. Overloading swab can remove this restriction and
	 *  may improve performance for custon types.
	 */
	template <typename T>
	inline T hostToBe(const T& value)
	{
#ifdef PT_LE
		return swap(value);
#else
		return value;
#endif
	}

	/** @brief Changes the byteorder of a given value from big-endian to host-byteorder.
	 *
	 *  @param value The value in host-byteorder
	 *  @return The value in big-endian
	 *
	 *  This function does nothing on a BE system, but calls swap() on a LE system.
	 *  The generic swap() function expects the type passed in to be bitwise-copyable
	 *  and thus does this function. Overloading swab can remove this restriction and
	 *  may improve performance for custon types.
	 */
	template <typename T>
	inline T beToHost(const T& value)
	{
#ifdef PT_LE
		return swap(value);
#else
		return value;
#endif
	}




	/** @brief In-place swab a type byte-wise.
	 *
	 *  This function could be used when a specialized function to swap a particular
	 *  data type is not exist. This function has a lot of additional overhead.
	 */
	inline uint8_t* swabUnaligned(uint8_t* data, size_t size)
	{
		assert( (size/2)*2 == size );

		for(size_t idx = 0; idx < size/2; ++idx) {
			const uint8_t tmp = data[idx];
			const size_t  pos = size-idx-1;

			data[idx] = data[pos];
			data[pos] = tmp;
		}

		return data;
	}

} // namespace Pt

#endif

