/*
  Copyright (C) 2008 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_API_CORE_H
#define PT_API_CORE_H

/** @defgroup Pt-Core Core Module

    @brief Basic types, memory, text, callbacks, utilities and serialization.

    The core module is the basis for all other modules of the framework. It
    has no dependency on any system specific libraries except the standard
    C++ library. Applications and other modules build on the areas covered
    by the core module:

    - @ref Pt-Basics provides program options, application settings, a
      type-safe event base class and a few general-purpose helper types.
    - @ref Pt-IntTypes provides fixed-size, platform-independent integer
      types.
    - @ref Pt-DateTime provides durations and points in calendar time.
    - @ref Pt-Atomics provides lock-free atomic integer and pointer
      operations.
    - @ref Pt-Coroutines provides cancellable, co_await-able tasks and
      lazily produced value sequences.
    - @ref Pt-Allocator provides custom allocation strategies for optimized
      memory management.
    - @ref Pt-Text extends the string and localization support of the C++
      standard library to work with unicode characters, strings, encodings
      and regular expressions.
    - @ref Pt-Signals provides a type-safe callback mechanism for connecting
      signals to slots and delegates.
    - @ref Pt-TypeTraits provides compile-time type traits and runtime type
      information.
    - @ref Pt-Convert provides conversion between strings, numbers and byte
      orders.
    - @ref Pt-Serialization composes and decomposes types to text or binary
      formats.
*/

/** @defgroup Pt-Basics Basic Types

    @ingroup Pt-Core
*/

/** @defgroup Pt-IntTypes Fixed-Size Integers

    @ingroup Pt-Core
*/

/** @defgroup Pt-DateTime Dates and Times

    @ingroup Pt-Core
*/

/** @defgroup Pt-Atomics Atomic Operations

    @ingroup Pt-Core
*/

/** @defgroup Pt-Coroutines Coroutines

    @ingroup Pt-Core
*/

/** @defgroup Pt-Allocator Allocators

    @ingroup Pt-Core
*/

/** @defgroup Pt-Text Text Processing

    @ingroup Pt-Core
*/

/** @defgroup Pt-Signals Signals and Delegates

    @ingroup Pt-Core
*/

/** @defgroup Pt-TypeTraits Type Traits and Information

    @ingroup Pt-Core
*/

/** @defgroup Pt-Convert Conversions

    @ingroup Pt-Core
*/

/** @defgroup Pt-Serialization Serialization

    @ingroup Pt-Core
*/

#endif
