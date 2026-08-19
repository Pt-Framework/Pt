/*
  Copyright (C) 2005 by Dr. Marc Boris Duerner

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
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#ifndef Pt_Method_h
#define Pt_Method_h

#include <Pt/Callable.h>
#include <Pt/Connectable.h>
#include <Pt/Slot.h>

namespace Pt {

/** @brief Adapter for class methods.

	The %Method class wraps member functions as Callable objects
	so that they can be used with the signals/slots framework.

	@ingroup Pt-Signals
*/
template <typename R, typename ClassT, typename... As>
class Method : public Callable<R, As...>
{
	public:
		/** @brief The wrapped member function signature.
		*/
		typedef R (ClassT::*MemFuncT)(As...);

	public:
		/** @brief Wraps the given object/member pair.
		*/
		Method(ClassT& object, MemFuncT method)
		: _object(&object)
		, _method(method)
		{ }

		/** @brief Returns a reference to this object's wrapped ClassT object.
		*/
		ClassT& object()
		{
			return *_object;
		}

		/** @brief Returns a const reference to the wrapped ClassT object.
		*/
		const ClassT& object() const
		{
			return *_object;
		}

		/** @brief Returns a reference to the wrapped member function.
		*/
		const MemFuncT& method() const
		{
			return _method;
		}

		// inherit doc
		R call(As... args) const
		{
			return (_object->*_method)(args...);
		}

		// inherit doc
		void invoke(As... args) const
		{
			(_object->*_method)(args...);
		}

		// inherit doc
		Method* clone() const
		{
			return new Method(*this);
		}

		Method& operator=(const Method& other)
		{
			_object = other._object;
			_method = other._method;
			return *this;
		}

		/** @brief Returns true if both use the same object and function pointer
		*/
		bool operator==(const Method& other) const
		{
			return _object == other._object && _method == other._method;
		}

	private:
		ClassT* _object;
		MemFuncT _method;
};

/** @brief Wraps %Method objects so that they can act as Slots.

	@ingroup Pt-Signals
*/
template <typename R, typename ClassT, typename... As>
class MethodSlot : public BasicSlot<R, As...>
{
	public:
		/** @brief Constructs from callable
		*/
		explicit MethodSlot(const Method<R, ClassT, As...>& method)
		: _method(method)
		{ }

		// inherit doc
		Slot* clone() const
		{
			return new MethodSlot(*this);
		}

		// inherit doc
		virtual const Callback* callable() const
		{
			return &_method;
		}

		// inherit doc
		virtual void onConnect(const Connection& connection)
		{
			_method.object().onConnectionOpen(connection);
		}

		// inherit doc
		virtual void onDisconnect(const Connection& connection)
		{
			_method.object().onConnectionClose(connection);
		}

		// inherit doc
		virtual bool equals(const Slot& slot) const
		{
			const MethodSlot* other = dynamic_cast<const MethodSlot*>(&slot);
			return other && _method == other->_method;
		}

	private:
		Method<R, ClassT, As...> _method;
};

/** @brief Returns a %Method object for the given object/method pair.

	@related Method
	@related Callable
*/
template <typename R, typename BaseT, typename ClassT, typename... As>
Method<R, ClassT, As...> callable(ClassT& object, R (BaseT::*method)(As...))
{
	return Method<R, ClassT, As...>(object, method);
}

/** @brief Returns a slot object for the given object/member pair.

	@related MethodSlot
	@related Slot
*/
template <typename R, typename BaseT, typename ClassT, typename... As>
MethodSlot<R, ClassT, As...> slot(ClassT& object, R (BaseT::*method)(As...))
{
	return MethodSlot<R, ClassT, As...>(callable(object, method));
}

} // namespace Pt

#endif
