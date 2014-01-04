/*
 * Copyright (C) 2004-2011 Marc Boris Duerner 
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

#define PT_VERSION_MAJOR 1
#define PT_VERSION_MINOR 0
#define PT_VERSION_REVISION 0

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

/** @defgroup Allocator Allocators

    The Allocator interface can be used to optimize or customize allocation
    strategies. Two allocators are provided, which can be approached by the
    Allocator interface, a pool based allocator and a page based allocator.
    A pool based allocator is beneficial in all cases where many small objects
    of small sizes are created. This is for example used to optimize memory
    usage during serialization. The page based allocator simply places data
    consecutively in memory and frees the whole block when its no longer in
    use. This is useful in situation where chunks of memory or objects are
    created and destroyed at the same time. 
*/

/** @defgroup CoreTypes Core Types

    The framework provides a number of basic types like fixed-size integers
    or a type named Any to contain values of different types. There is also
    functionality for type conversions and to swap or adjust byteorders. The
    type traits are useful for generic programming to query information about
    a type at compile time to adapt it or specialize for it. A wrapper for
    std::type_info makes it easier to store and compare type information.
*/

/** @defgroup DateTime Dates and Times

    The Pt core module contains some simple types to represent times and
    dates. They can be used for comparison, sorting and in calculations
    including other dates, times and timespans.
*/

/** @defgroup Unicode Text Processing

    This set of classes and functions extends the string and localization
    support of the C++ standard library to work with unicode characters and
    strings. A unicode character type and string class (a specialization of
    std::basic_string) can be used to hold unicode text. A set of functions
    allows to transform and classify individual characters. Text can be 
    converted e.g. between different encodings using i/o streams and text
    codecs. A regular expression class allows to search and match patterns 
    in unicode strings. Localization facets are available for the systems
    which support standard C++ locales.
*/

/** @defgroup Serialization

    Data structures and types can be serialized to text or binary formats using
    Pt's serialization. This is used within the framework to load and store
    data or to implement remote procedure calls. It is extensible to work with
    all kinds of types, including STL containers, PODs (plain old data types),
    buitlin language types or custom data types. The framework separates the 
    process of composing and decomposing types from the formatting stage,
    resulting in a two-phase serialization process. This also allows to resolve
    and fixup shared pointers or references. 
    
    A type is serializable, if two operators are implemented to compose and 
    decompose it to a SerializationInfo. The SerializationContext provides
    improved memory management, a mechanism to generate IDs for shared pointers
    and a way to further customize or override serialization for a type.
    Alternatively, performance can be increased by implementing a Composers
    or Decomposer for the type, however it is more complicated to do so.
    
    Various formats are supported by implementing Formatters. Other modules
    of the framework also implement Formatters, for example to support 
    serialization to XML. The Serializer and Deserializer combine a Formatter
    and a SerializationContext, manage composition and decomposition and thus
    form the high-level interface for the serialization of a set of types.
*/

/** @defgroup sigslot Signals and Slots
*/

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

