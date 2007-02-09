/***************************************************************************
 *   Copyright (C) 2004-2006 Marc Boris Dürner                             *
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

#if !defined(PT_LE) && !defined(PT_BE)

	#if __BYTE_ORDER == __LITTLE_ENDIAN
		#define PT_LE

	#elif __BYTE_ORDER == __BIG_ENDIAN
		#define PT_BE

	#elif defined(_MSC_VER)
		#define PT_LE

	#else
		#error "PT_LE or PT_BE needs to be defined."

	#endif

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
#else
		return ( (value & 0x00FF) << 8 ) |
					 ( (value & 0xFF00) >> 8 );
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
#else
		return ( (value & 0x000000FF) << 24 ) |
					 ( (value & 0x0000FF00) <<  8 ) |
					 ( (value & 0x00FF0000) >>  8 ) |
					 ( (value & 0xFF000000) >> 24 );
#endif
	}

#ifdef PT_64BIT
	/** @brief Swaps the byteorder of the given 64-bit value.
	 *  @internal
	 */
	template <typename T>
	inline T swab64(T value)
	{
#ifdef USE_BYTE_MOVE
		uint8_t *w = reinterpret_cast<uint8_t*>(&value);
		const uint8_t w0 = w[0];
		const uint8_t w1 = w[1];
		const uint8_t w2 = w[2];
		const uint8_t w3 = w[3];
		const uint8_t w4 = w[4];
		const uint8_t w5 = w[5];
		const uint8_t w6 = w[6];
		const uint8_t w7 = w[7];
		w[0] = w7;
		w[1] = w6;
		w[2] = w5;
		w[3] = w4;
		w[4] = w3;
		w[5] = w2;
		w[6] = w1;
		w[7] = w0;
		return(value);
#else
		return ( (value & 0x00000000000000FFULL) << 56 ) |
				( (value & 0x000000000000FF00ULL) << 40 ) |
				( (value & 0x0000000000FF0000ULL) << 24 ) |
				( (value & 0x00000000FF000000ULL) <<  8 ) |
				( (value & 0x000000FF00000000ULL) >>  8 ) |
				( (value & 0x0000FF0000000000ULL) >> 24 ) |
				( (value & 0x00FF000000000000ULL) >> 40 ) |
				( (value & 0xFF00000000000000ULL) >> 56 );
#endif
	}
#endif




	/** @brief Dummy function which does nothing.
	 *
	 *  Just for the sake of completeness.
	 */
	inline int8_t swab(int8_t value)
	{ return value; }

	/** @brief Dummy function which does nothing.
	 *
	 *  Just for the sake of completeness.
	 */
	inline uint8_t swab(uint8_t value)
	{ return value; }


	/** @brief Swaps the byteorder of an int16_t.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline int16_t swab(int16_t value)
	{ return swab16(value); }

	/** @brief Swaps the byteorder of a uint16_t.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline uint16_t swab(uint16_t value)
	{ return swab16(value); }


	/** @brief Swaps the byteorder of an int32_t.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline int32_t swab(int32_t value)
	{ return swab32(value); }

	/** @brief Swaps the byteorder of a uint32_t.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline uint32_t swab(uint32_t value)
	{ return swab32(value); }


#ifdef PT_64BIT
	/** @brief Swaps the byteorder of an int64_t.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline int64_t swab(int64_t value)
	{ return swab64(value); }

	/** @brief Swaps the byteorder of a uint64t.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline uint64_t swab(uint64_t value)
	{ return swab64(value); }
#endif


	/** @brief Swaps the byteorder of a float.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline float swab(float value)
	{
		const uint32_t &p = *reinterpret_cast<const uint32_t*>(&value);
		const uint32_t  s = swab32(p);
		return *reinterpret_cast<const float*>(&s);
	}

	/** @brief Swaps the byteorder of a double.
	 *
	 *  @param value The value to be byte-swapped
	 *  @return The byte-swapped value
	 *
	 *  Overloads the generic swap().
	 */
	inline double swab(double value)
	{
		const uint64_t &p = *reinterpret_cast<const uint64_t*>(&value);
		const uint64_t  s = swab64(p);
		return *reinterpret_cast<const double*>(&s);
	}




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
		return swab(value);
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
		return swab(value);
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
		return swab(value);
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
		return swab(value);
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
		const size_t size_min1 = size - 1;
		const size_t size_div2 = size / 2;

		assert( size_div2*2 == size );

		for(size_t idx = 0; idx < size_div2; ++idx) {
			const uint8_t tmp = data[idx];
			const size_t  pos = size_min1 - idx;

			data[idx] = data[pos];
			data[pos] = tmp;
		}

		return data;
	}

} // namespace Pt

#endif

