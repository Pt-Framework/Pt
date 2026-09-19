/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#if defined(_MSC_VER)
    //http://support.microsoft.com/support/kb/articles/Q134/9/80.asp
    //http://support.microsoft.com/support/kb/articles/Q168/9/58.ASP
    //http://support.microsoft.com/support/kb/articles/Q172/3/96.ASP

    // deprectated stdc++ functions
    #pragma warning( disable : 4996 )

    // dll-linkage
    #pragma warning( disable : 4251 )

    // non dll-interface base class
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

#if defined(WIN32) || defined(_WIN32)
    // suppress min/max macros from win32 headers
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
#endif

#define PT_VERSION_MAJOR 2
#define PT_VERSION_MINOR 0
#define PT_VERSION_REVISION 0
#define PT_VERSION_PRERELEASE 1

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32)
    #define PT_EXPORT __declspec(dllexport)
    #define PT_IMPORT __declspec(dllimport)
#elif __GNUC__ >= 4
    #define PT_EXPORT __attribute__((visibility("default")))
    #define PT_IMPORT
#elif __SYMBIAN32__
    #define PT_EXPORT
    #define PT_IMPORT
#else
    #define PT_EXPORT
    #define PT_IMPORT
#endif

#if defined(PT_API_EXPORT)
    #define PT_API PT_EXPORT
#else
    #define PT_API PT_IMPORT
#endif

#if ! defined(__NOLOCK_ON_INPUT)
    // disable locking of iostreams on xlC
    #define __NOLOCK_ON_INPUT
#endif

#if ! defined(__NOLOCK_ON_OUTPUT)
    // disable locking of iostreams on xlC
    #define __NOLOCK_ON_OUTPUT
#endif

#ifndef Pt_Api_h
#define Pt_Api_h

/** @namespace Pt
    @brief Core module.

    This module is the basis for all other modules of the framework It has
    no dependency to any system specific libraries except the standard c++
    library. It provides some basic types, support for byte-order handling,
    atomic integer operations, type-traits, an unicode string and character
    class, serialization and a signals/delegates based callback mechanism.
*/
namespace Pt {

    class Allocator;
    class Any;
    class Connectable;
    class Connection;
    class Date;
    class DateTime;
    class Event;
    class SerializationContext;
    class SerializationInfo;
    class Settings;
    class SourceInfo;
    class Time;

}

#endif
