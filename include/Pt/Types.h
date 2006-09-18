/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
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
#ifndef Pt_Types_h
#define Pt_Types_h

#include <Pt/Api.h>

#include <climits>
#include <cstddef>


namespace Pt {

	/// \exclude
	typedef unsigned char  uchar;
	/// \exclude
	typedef unsigned short ushort;
	/// \exclude
	typedef unsigned int   uint;
	/// \exclude
	typedef unsigned long  ulong;

	/// \exclude
	typedef std::ptrdiff_t ssize_t;

	/// \exclude
	using std::size_t;

	/// \exclude
	typedef signed char int8_t;
	/// \exclude
	typedef unsigned char uint8_t;

#if USHRT_MAX == 0xffff
	typedef short int16_t;
	typedef unsigned short uint16_t;
#elif UINT_MAX == 0xffff
	typedef int int16_t;
	typedef unsigned int uint16_t;
#elif ULONG_MAX == 0xffff
	typedef long int16_t;
	typedef unsigned long uint16_t;
#endif

#if USHRT_MAX == 0xffffffffUL
	typedef short int32_t;
	typedef unsigned short uint32_t;
#elif UINT_MAX == 0xffffffffUL
	typedef int int32_t;
	typedef unsigned int uint32_t;
#elif ULONG_MAX == 0xffffffffUL
	typedef long int32_t;
	typedef unsigned long uint32_t;
#endif

#if UINT_MAX == 18446744073709551615UL
	#define PT_64BIT 1
	typedef int int64_t;
	typedef unsigned int uint64_t;
#elif ULONG_MAX == 18446744073709551615UL
	#define PT_64BIT 1
	typedef long int64_t;
	typedef unsigned long uint64_t;
#elif ULLONG_MAX == 18446744073709551615ULL
	#define PT_64BIT 1
	typedef long long int64_t;
	typedef unsigned long long uint64_t;
#endif

} // namespace Pt

#endif
