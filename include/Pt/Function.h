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

#ifndef Pt_Function_h
#define Pt_Function_h

#include <Pt/Callable.h>
#include <Pt/Slot.h>

namespace Pt {

/** @brief Wraps free functions into a generic callable for use with the signals/slots

	The %Function class wraps free functions in the form of a Callable,
	for use with the signals/slots framework.

	@ingroup sigslot
*/
template <typename R, typename... As>
class Function : public Callable<R, As...>
{
	public:
		/** @brief The function signature wrapped by this class
		*/
		typedef R (*FuncT)(As...);

	public:
		/** @brief Construct from function pointer
		*/
		explicit Function(FuncT function)
		: _function(function)
		{ }

		R operator()(As... args) const
		{
			return (*_function)(args...);
		}

		Function* clone() const
		{
			return new Function(*this);
		}

		/** @brief Returns true if both use the same function pointer
		*/
		bool operator==(const Function& other) const
		{
			return _function == other._function;
		}

	private:
		FuncT _function;
};

/** @brief Wraps %Function objects so that they can act as slots.

	@ingroup sigslot
*/
template <typename R, typename... As>
class FunctionSlot : public BasicSlot<R, As...>
{
	public:
		/** @brief Constructs from callable
		*/
		explicit FunctionSlot(const Function<R, As...>& function)
		: _function(function)
		{ }

		Slot* clone() const
		{
			return new FunctionSlot(*this);
		}

		virtual const void* callable() const
		{
			return &_function;
		}

		virtual void onConnect(const Connection&)
		{ }

		virtual void onDisconnect(const Connection&)
		{ }

		virtual bool equals(const Slot& slot) const
		{
			const FunctionSlot* other = dynamic_cast<const FunctionSlot*>(&slot);
			return other && _function == other->_function;
		}

	private:
		Function<R, As...> _function;
};

/** @brief Returns a %Function wrapper for the given free/static function.

	@related Function
	@related Callable
*/
template <typename R, typename... As>
Function<R, As...> callable(R (*function)(As...))
{
	return Function<R, As...>(function);
}

/** @brief Returns a slot object for the given free/static function.

	@related FunctionSlot
	@related Slot
*/
template <typename R, typename... As>
FunctionSlot<R, As...> slot(R (*function)(As...))
{
	return FunctionSlot<R, As...>(callable(function));
}

} // namespace Pt


#endif // Pt_Function_h
