/*
 * Copyright (C) 2004-2007 Marc Boris Duerner
 * Copyright (C) 2006 by Aloysius Indrayanto
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
#ifndef Pt_Api_h
#define Pt_Api_h

#if defined(_MSC_VER)
	//http://support.microsoft.com/support/kb/articles/Q134/9/80.asp
	//http://support.microsoft.com/support/kb/articles/Q168/9/58.ASP
	//http://support.microsoft.com/support/kb/articles/Q172/3/96.ASP

	// deprectated stdc++ functions
	#pragma warning( disable : 4996 )

	// dll-linkage
	#pragma warning( disable : 4251 )

	//non dll-interface base class
	#pragma warning( disable : 4275 )

	// exception sepcification ignored
	#pragma warning( disable : 4290 )
#endif

#if defined (__INTEL_COMPILER)
    // field of class type without a DLL interface used in a class with a DLL interface
    #pragma warning( disable : 1744 )

    //base class dllexport/dllimport specification differs from that of the derived class
    #pragma warning( disable : 1738 )
#endif

#ifdef _WIN32_WCE
    // WinCE does not provide locale-classes
#else
    #define PT_WITH_STD_LOCALE 1
#endif
 
#if defined(WIN32) || defined(_WIN32)
    // suppress min/max macros from win32 headers
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif

    // Use of features specific Windows versions
    #ifndef WINVER
    #define WINVER 0x0501
    #endif

    #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501
    #endif

    #ifndef _WIN32_WINDOWS
    #define _WIN32_WINDOWS 0x0410
    #endif
#endif

#define PT_VERSION_MAJOR 1
#define PT_VERSION_MINOR 0
#define PT_VERSION_REVISION 0

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32)
    #define PT_EXPORT __declspec(dllexport)
    #define PT_IMPORT __declspec(dllimport)
    #if !defined(_NATIVE_WCHAR_T_DEFINED)
        #define PT_WCHAR_T_IS_USHORT
    #endif
#elif __GNUC__ >= 4
    #define PT_EXPORT __attribute__((visibility("default")))
    #define PT_IMPORT
#elif __SYMBIAN32__
    // leave that empty on symbian for now
    // dynamic linking is not yet supported
    #define PT_EXPORT
    #define PT_IMPORT
    #define PT_WCHAR_T_IS_USHORT
#else
    #define PT_EXPORT
    #define PT_IMPORT
#endif

#if defined(PT_API_EXPORT)
    #define PT_API PT_EXPORT
#else
    #define PT_API PT_IMPORT
#endif

#if !defined(__NOLOCK_ON_INPUT)
    // disable locking of iostreams on xlC
    #define __NOLOCK_ON_INPUT
#endif

#if !defined(__NOLOCK_ON_OUTPUT)
    // disable locking of iostreams on xlC
    #define __NOLOCK_ON_OUTPUT
#endif


/** @defgroup DateTime
    @brief Dates and Times

    The Pt core module contains some simple types to represent times and
    dates. They can be used for comparison, sorting and in calculations
    including other dates, times and timespans.
*/

/** @defgroup Unicode
    @brief Unicode string handling

    Pt extends the string handling of the standard C++ library by
    a unicode capable character type and a specialization of the
    std:basic_string<> template for this charcter type. Localization
    factes are provides that allow the use of i/o streams with the
    unicode charcter type.
*/

/** @namespace Pt
    @brief Core module

    This module is the base module for all other modules and has no dependency
    to any system specific libraries except the standard c++ library. It
    provides some basic types, support for byte-order handling, atomic integer
    operations, type-traits, an unicode string and character class, serialization
    and a signals/delegates based callback mechanism.
*/
namespace Pt {

    class Any;
    class Connectable;
    class Connection;
    class SourceInfo;
    class NonCopyable;
}

/*______ _       _   _                         _____
  | ___ \ |     | | (_)                       /  __ \ _     _
  | |_/ / | __ _| |_ _ _ __  _   _ _ __ ___   | /  \/| |_ _| |_
  |  __/| |/ _` | __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
  | |   | | (_| | |_| | | | | |_| | | | | | | | \__/\|_|   |_|
  \_|   |_|\__,_|\__|_|_| |_|\__,_|_| |_| |_|  \____/
  ______                                           _
  |  ___|                                         | |
  | |_ _ __ __ _ _ __ ___   _____      _____  _ __| | __
  |  _| '__/ _` | '_ ` _ \ / _ \ \ /\ / / _ \| '__| |/ /
  | | | | | (_| | | | | | |  __/\ V  V / (_) | |  |   <
  \_| |_|  \__,_|_| |_| |_|\___| \_/\_/ \___/|_|  |_|\_\
*/

#endif
