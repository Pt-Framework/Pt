/***************************************************************************
 *   Copyright (C) 2004-2006 Marc Boris Dürner                             *
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

#include <Pt/Api.h>
#include <Pt/Types.h>


namespace Pt
{

//! \internal @brief Swaps the byteorder of a given 16-bit value.
template <typename T>
inline T swab16(T value) {
	return ((value & 0x00ff) << 8) |
	       ((value & 0xff00) >> 8);
}


//! \internal @brief Swaps the byteorder of a given 32-bit value.
template <typename T>
inline T swab32(T value) {
	return ((value & 0x000000ff) << 24) |
	       ((value & 0x0000ff00) << 8)  |
	       ((value & 0x00ff0000) >> 8)  |
	       ((value & 0xff000000) >> 24);
}


#ifdef PT_64BIT

//! \internal @brief Swaps the byteorder of a given 64-bit value.
template <typename T>
inline T swab64(T value) {
	return ((value & 0x00000000000000ffULL) << 56) |
	       ((value & 0x000000000000ff00ULL) << 40) |
	       ((value & 0x0000000000ff0000ULL) << 24) |
	       ((value & 0x00000000ff000000ULL) << 8)  |
	       ((value & 0x000000ff00000000ULL) >> 8)  |
	       ((value & 0x0000ff0000000000ULL) >> 24) |
	       ((value & 0x00ff000000000000ULL) >> 40) |
	       ((value & 0xff00000000000000ULL) >> 56);
}

#endif


//! @brief In-place swab a type byte-wise
inline uint8_t* swabUnaligned( uint8_t* data, size_t size)
{
	uint8_t buf;
	const size_t n = (size/2);
	for(size_t i = 0; i < n;  ++i)
	{
		buf					= data[i];
		data[i]				= data[size -  i - 1];
		data[size - i - 1]	= buf;
	}

	return data;
}


//! @brief Swaps the byteorder of a value
/**
    The swab() function is used by the high-level byteorder conversion functions
    such as Pt::leToHost() to reverse the byte-order of a type if neccessary,
    This generic version simply reverses the byte-order zsing pointer arithmetics,
    thus the type must be bitwise copyable. This function should be specialised/overloaded
    for custom types and overloaded swab()'s are provided for many POD types already.

    @param value the value to be swabed
    @return A swabed copy of the value passed in
*/
template <typename T>
inline T swab(const T& value)
{
    T tmp = value;
    swabUnaligned( (uint8_t*)(&tmp) , sizeof(T) );
    return tmp;
}


//! @brief dummy
/**
    Just for the sake of completeness.
*/
inline uint8_t swab(uint8_t value)
{ return value; }


//! @brief dummy
/**
    Just for the sake of completeness.
*/
inline int8_t swab(int8_t value)
{ return value; }


//! @brief Swaps the byteorder of a uint16_t.
/**
	Overloads the generic swab().

    @param value The value to be byte-swapped
    @return The byte-swapped value
*/
inline uint16_t swab(uint16_t value)
{
	return swab16(value);
}


//! @brief Swaps the byteorder of a int16_t.
/**
	Overloads the generic swab().

    @param value The value to be byte-swapped
    @return The byte-swapped value
*/
inline int16_t swab(int16_t value)
{
	return swab16(value);
}


//! @brief Swaps the byteorder of a uint32_t.
/**
	Overloads the generic swab().

    @param value The value to be byte-swapped
    @return The byte-swapped value
*/
inline uint32_t swab(uint32_t value)
{
	return swab32(value);
}


//! @brief Swaps the byteorder of a int32_t.
/**
	Overloads the generic swab().

    @param value The value to be byte-swapped
    @return The byte-swapped value
*/
inline int32_t swab(int32_t value)
{
	return swab32(value);
}


#ifdef PT_64BIT

//! @brief Swaps the byteorder of a uint64_t.
/**
	Overloads the generic swab().

    @param value The value to be byte-swapped
    @return The byte-swapped value
*/
inline uint64_t swab(uint64_t value)
{ return swab64(value); }


//! @brief Swaps the byteorder of a int64_t.
/**
	Overloads the generic swab().

    @param value The value to be byte-swapped
    @return The byte-swapped value
*/
inline int64_t swab(int64_t value)
{ return swab64(value); }

#endif


//! @brief Changes the byteorder of a given value from host-byteorder to little-endian.
/**
	This function does nothing on a LE system, but calls swab() on a BE system. The
	generic swab() function expects	the type passed in to be bitwise-copyable
	and thus does this function. Overloading swab can remove this restriction and may
	improve performance for custon types.

    @param value The value in host-byteorder
    @return The value changed to little-endian
*/
template <typename T>
inline T hostToLe(const T& value)
{
#ifdef PT_LE
	return value;
#elif PT_BE
	return swab(value);
#else
	#error "PT_LE or PT_BE needs to be defined."
#endif
}


//! @brief Changes the byteorder of a given value from little-endian to host-byteorder.
/**
	This function does nothing on a LE system, but calls swab() on a BE system. The
	generic swab() function expects	the type passed in to be bitwise-copyable
	and thus does this function. Overloading swab can remove this restriction and may
	improve performance for custon types.

    @param value The little-endian value
    @return The value changed to host-byteorder
*/
template <typename T>
inline T leToHost(const T& value)
{
#ifdef PT_LE
	return value;
#elif PT_BE
	return swab(value);
#else
	#error "PT_LE or PT_BE needs to be defined."
#endif
}


//! @brief Changes the given value from the host-byteorder to big-endian.
/**
	This function does nothing on a BE system, but calls swab() on a LE system. The
	generic swab() function expects	the type passed in to be bitwise-copyable
	and thus does this function. Overloading swab can remove this restriction and may
	improve performance for custon types.

    @param value The value in host-byteorder
    @return The value in big-endian
*/
template <typename T>
inline T hostToBe(const T& value)
{
#ifdef PT_LE
	return swab(value);
#elif PT_BE
	return value;
#else
	#error "PT_LE or PT_BE needs to be defined."
#endif
}


//! @brief Changes the byteorder of a given value from big-endian to host-byteorder.
/**
	This function does nothing on a BE system, but calls swab() on a LE system. The
	generic swab() function expects	the type passed in to be bitwise-copyable
	and thus does this function. Overloading swab can remove this restriction and may
	improve performance for custon types.

    @param value The big-endian value
    @return The value changed to host-byteorder
*/
template <typename T>
inline T beToHost(const T& value)
{
#ifdef PT_LE
	return swab(value);
#elif PT_BE
	return value;
#else
	#error "PT_LE or PT_BE needs to be defined."
#endif
}

} // namespace Pt

#endif

