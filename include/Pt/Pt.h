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
This module is the core module of the Platinum framework. It provides
basic types and support for byte-order handling, support for object-modelling,
and the exception hierachy root. The core module introduces the namespace Pt.
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
	class TypeInfo;
	
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
	class TypeTraits;

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

	class Void;
	class SourceInfo;
	class NonCopyable;

}

#endif
