/***************************************************************************
 *   Copyright (C) 2004-2006 by Marc Boris Dürner                          *
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
 **************************************************************************/

#ifndef Pt_h
#define Pt_h

/** \dir
!!! Pt (module)
\n
!! The core module
\n
This module is the base module for all other modules. It provides
basic types, support for byte-order handling and macros for conditional
compilation. Support for object-modelling is included as well as the
root of the exception hierachy.
The core module introduces the namespace ptv and all extention modules
use their own module-wide namespace i.e.The ptv-io module uses the
ptv::io namespace.
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

/** \page "Basic Types"
!!!Basic Types
The core module defines the following plain data types of fixed size:
|int8_t|uint8_t
|int16_t|uint16_t
|int32_t|uint32_t
|int64_t|uint64_t

For convenience special typedefs for unsigned plain datatypes are
available:
||typedef || type
|uchar|unsigned char
|ushort|unsigned short
|uint|unsigned int
|ulong|unsigned long
*/

/** \page "Byteorder  algorithms"
!!! Byteorder algorithms
The byteoder API consists of two layers. One layer is responsible for
swapping the byteorder of a type and the high-level API can be used to
convert from an exernal byteorder to the host byteorder.

!!Swapping the byteorder

The low-level API consists of a generic swab() function, which is able to
swap the byteorder of a type by bytewise copying. This works for all
bitwise copyable types. Though, for many types, byteorder sapping can be
implemented with better performance. Therefore overloads of swab() are
offered for plain data types such as uint16_t that use shift operations.

The swab() function can be overloaded for custom types as well. For example
the following struct can have an overloaded swab():

@code
struct value
{
	value(char a, uint16_t b, char c)
	: _a(a), _b(b), _c(c)
	char _a;
	uint16_t _b;
	char _c;
};

value swab(const value& val)
{
	return value(val._a, swab(val.b), val._c);
}
@endcode

!!Converting from external byteorder to the host byteorder
The low-level swab() algorithm is used by the high level functions for
external to host byteorder conversions, for example beToHost(). On a little
endian system this function needs to swap the byteorder, on a big endian
system this funtion should return the value unchanged. The macros PTV_LE
and PRV_BE define whether the code should be compiled for a little endian
or big endian machine and adjust the behaviour of the high-level conversion
functions. If a swab() algorithm is overloaded for a custom type it can
be directly used with the high level functions:

@code
// some value in big endian byteorder
value beVal = getBeValue();

// convert to host byteorder
value hostVal = beToHost(beVal);
@endcode
*/

/** \page "Error and Exception Handling"
!!! Error and Exception Handling
All exceptions thrown by PPR are derived from std::exception, to fit well
in the context of the C++ standard library. Similarily, the two main
exception types are RuntimeError and LogicError.
A RuntimeError can for example occur if an underlying file descriptor
suddenly becomes invalid. These errors are usually out of the scope of
the application and not easily avoidable. Sometimes the best the application
can do is to shut down properly before data becomes corrupted.
LogicErrors on the other hand are in the domain of the application. Often
they are preventable and an example of a typical LogicError is if an invalid
argument such as a malformed URL is passes to a function. IT is most likely
that the program will just want to retry the operation under different
conditions.
The SourceInfo class plays a special role in PPR's error handling, since
all exception classes are constructed with a SourceInfo object indicating
the location where the exception was thrown. The code below will print
the function name, the source file and the line where the esxception was thrown:

@code
try {
	// monitored code
}
catch(ptv::Exception& e) {
	cerr << "Exception in " << e.sourceInfo().func() << endl;
	     << e.sourceInfo().file() << ": " << e.sourceInfo().line() << endl;
	     << e.what() << endl;
}
@endcode

Most modules add new exception classes to this hierachy.
*/

/** \page "Object Modeling"
!! Object Modeling
PPR supports various ways to help model your objects. This begins with simple
mixin classes like the NonCopyable to disallow copies of a derived class or the
Singleton to implement your class as a singleton.
*/

#endif
