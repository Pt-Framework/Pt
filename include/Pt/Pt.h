/*
 * Copyright (C) 2004-2006 by Marc Boris Duerner
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

#ifndef Pt_h
#define Pt_h

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

    template <typename T>
    struct AnyTraits;

    template < typename R,
                typename A1,
                typename A2,
                typename A3,
                typename A4,
                typename A5 >
    class Callable;

    class Connectable;
    class Connection;
    class ICallable;

    // NOTE: change when API is ready
    //class PropertyProxy
    //class Property;

    class Time;

    template < typename R,
                typename C,
                typename A1,
                typename A2,
                typename A3,
                typename A4,
                typename A5 >
    class ConstMethod;

    template <typename T>
    struct TypeInfo;

    class Date;

    template < typename A1,
                typename A2,
                typename A3,
                typename A4,
                typename A5 >
    class Invokable;

    class RefCounted;
    class Application;
    class DateTime;
    class Reflectable;

    template <typename T>
    struct TypeTraits;

    template <typename T>
    class Arg;

    template < typename R,
                typename A1,
                typename A2,
                typename A3 >
    class Delegate;

    template < typename A1,
                typename A2,
                typename A3 >
    class Signal;

    class Args;
    class Event;

    template <typename T, typename AllocatorT >
    class Singleton;

    class Variant;
    class AtomicInt;
    class Exception;
    class Slot;

    template < typename R,
                typename A1,
                typename A2,
                typename A3,
                typename A4,
                typename A5 >
    class BasicSlot;

    template <typename T>
    class VariantTraits;

    template < typename R,
                typename A1,
                typename A2,
                typename A3 >
    class Function;

    template < typename R,
                typename C,
                typename A1,
                typename A2,
                typename A3,
                typename A4,
                typename A5 >
    class Method;

    template <typename T, typename ModelT >
    class SmartPtr;

    struct Void;
    class SourceInfo;
    class NonCopyable;
}

#endif
