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
	#define PT_ALIGN_BEGIN(value) __declspec( align( value ) )
	#define PT_ALIGN_END(value)
	#define PT_PACKED
#elif __GNUC__ >= 4
	#define PT_EXPORT __attribute__((visibility("default")))
	#define PT_IMPORT
	#define PT_ALIGN_BEGIN(value)
	#define PT_ALIGN_END(value) __attribute__ ((aligned ( value )))
	#define PT_PACKED __attribute__ ((packed))
#elif __GNUC__
	#define PT_EXPORT
	#define PT_IMPORT
	#define PT_ALIGN_BEGIN(value)
	#define PT_ALIGN_END(value) __attribute__ ((aligned ( value )))
	#define PT_PACKED __attribute__ ((packed))
#elif __BCPLUSPLUS__
	#error borland compiler at the time not supported
#else
	#error unknown compiler
#endif

#ifdef PT_DLL_API
	#define PT_API PTV_EXPORT
#else
	#define PT_API PTV_IMPORT
#endif


#endif
