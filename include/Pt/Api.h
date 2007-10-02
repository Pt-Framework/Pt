/***************************************************************************
 *   Copyright (C) 2004-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006 by Aloysius Indrayanto                             *
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
  \_| |_|  \__,_|_| |_| |_|\___| \_/\_/ \___/|_|  |_|\_\     */

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
    #define PT_WITHOUT_STD_LOCALE
#endif

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32)
    #define PT_EXPORT __declspec(dllexport)
    #define PT_IMPORT __declspec(dllimport)
    //#define PT_PACKED
    //#define PT_ALIGN(value) __declspec( align( value ) )

    // suppress min/max macros from win32 headers
    #ifndef NOMINMAX
    	#define NOMINMAX
    #endif

    // Modify the following defines if you have to target a platform prior to the ones specified below.
    // Refer to MSDN for the latest info on corresponding values for different platforms.
    // INFO: Taken from stdafx.h of a Visual Studio MFC project.
    #ifndef WINVER // Allow use of features specific to Windows XP or later.
    #define WINVER 0x0501 // Change this to the appropriate value to target other versions of Windows.
    #endif

    #ifndef _WIN32_WINNT // Allow use of features specific to Windows XP or later.
    #define _WIN32_WINNT 0x0501 // Change this to the appropriate value to target other versions of Windows.
    #endif

    #ifndef _WIN32_WINDOWS        // Allow use of features specific to Windows 98 or later.
    #define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
    #endif
#elif __GNUC__ >= 4
    #define PT_EXPORT __attribute__((visibility("default")))
    #define PT_IMPORT
    //#define PT_PACKED __attribute__ ((packed))
    //#define PT_ALIGN(value) __attribute__ ((aligned ( value )))
#elif __GNUC__
    #define PT_EXPORT
    #define PT_IMPORT
    //#define PT_PACKED __attribute__ ((packed))
    //#define PT_ALIGN(value) __attribute__ ((aligned ( value )))
#elif __xlC__
    #define PT_EXPORT
    #define PT_IMPORT
#elif __BCPLUSPLUS__
    #error "Borland compiler is not supported at this moment"
#else
    #error "Unknown compiler, can't continue"
#endif

#if defined(PT_API_EXPORT)
#    define PT_API PT_EXPORT
#  else
#    define PT_API PT_IMPORT
#  endif

#endif
