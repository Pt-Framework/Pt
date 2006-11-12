/***************************************************************************
 *   Copyright (C) 2004-2006 Marc Boris Drner                             *
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

#ifndef Pt_Api_h
#define Pt_Api_h


#ifdef _MSC_VER
	#pragma warning( disable : 4996 ) // deprectated stdc++ functions
	#pragma warning( disable : 4251 ) // dll-linkage
	#pragma warning( disable : 4290 ) // exception sepcification ignored
#endif

#ifdef _MSC_VER
	#define PT_EXPORT __declspec(dllexport)
	#define PT_IMPORT __declspec(dllimport)
	#define PT_PACKED
	#define PT_ALIGN(value) __declspec( align( value ) )

	#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
		#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
	#endif

	#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
		#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
	#endif

#elif __GNUC__ >= 4
	#define PT_EXPORT __attribute__((visibility("default")))
	#define PT_IMPORT
	#define PT_PACKED __attribute__ ((packed))
	#define PT_ALIGN(value) __attribute__ ((aligned ( value )))
#elif __GNUC__
	#define PT_EXPORT
	#define PT_IMPORT
	#define PT_PACKED __attribute__ ((packed))
	#define PT_ALIGN(value) __attribute__ ((aligned ( value )))
#elif __BCPLUSPLUS__
	#error borland compiler at the time not supported
#else
	#error unknown compiler
#endif

#ifdef PT_API_IMPORT
	#define PT_API PT_IMPORT
#else
	#define PT_API PT_EXPORT
#endif


#endif
