/***************************************************************************
 *   Copyright (C) 2004-2006 by Dr. Marc Boris Dürner                      *
 *   Copyright (C) 2005 Stephan Beal                                       *
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

#ifndef Pt_Signal_h
#define Pt_Signal_h

#include <Pt/Void.h>
#include <Pt/Function.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/Connectable.h>

#include <list>
#include <algorithm>
#include <iostream>


namespace Pt {

	/** Multicast Signal.
	    A Signal can be connected to multiple targets. The return
	    value of the target is ignored, when the signal is sent.
	*/
	template <typename A1 = Pt::Void, typename A2 = Pt::Void, typename A3 = Pt::Void>
	class Signal : public Connectable {
		public:
			typedef Pt::Invokable<A1, A2, A3> Invokable;

			struct Sentry {
				Sentry(const Signal& signal)
				: _signal(signal)
				{
					_signal._sending = true;
					_signal._cleanup = false;
				}

				~Sentry()
				{
					_signal._sending = false;
					if(_signal._cleanup)
					{
						_signal._connections.remove( Connection() );
						_signal._cleanup = false;
					}
				}

				const Signal& _signal;
			};

		public:
			Signal()
			: _sending(false)
			{ }

			Signal(const Signal& signal)
			: Connectable()
			, _sending(false)
			{
				Signal::operator=(signal);
			}

			~Signal()
			{ }

			Signal& operator=(const Signal& other)
			{
				Connectable::operator=(other);
				return *this;
			}

			virtual void opened(const Connection& c)
			{
				Connectable::opened(c);
			}

			virtual void closed(const Connection& c)
			{
				// if the signal is currently calling its slots, do not
				// remove the connection now, but just replace it by
				// an invalid one and set the cleanup flag to true.
				// Any invalid connection objects will be removed after
				// the signal has finished calling its slots.
				if(_sending)
				{
					std::list<Connection>::iterator it;
					it = std::find(this->connections().begin(), this->connections().end(), c );
					if( it != this->connections().end() )
					{
						*it = Connection();
						_cleanup = true;
					}
				}
				else
					Connectable::closed(c);
			}

			inline void send() const
			{
				Sentry sentry(*this);

				std::list<Connection>::const_iterator it = Connectable::connections().begin();
				for(; it != _connections.end(); ++it)
				{
					if( &( it->sender() ) != this || false == it->valid() )
						continue;

					const Invokable* invokable = static_cast<const Invokable*>( it->slot().callable() );
					invokable->invoke();
				}
			}

			template <typename Arg1>
			inline void send(Arg1 a1) const
			{
				Sentry sentry(*this);

				std::list<Connection>::const_iterator it = Connectable::connections().begin();
				for(; it != _connections.end(); ++it)
				{
					if( &( it->sender() ) != this || false == it->valid() )
						continue;

					const Invokable* invokable = static_cast<const Invokable*>( it->slot().callable() );
					invokable->invoke(a1);
				}
			}

			template <typename Arg1, typename Arg2>
			inline void send(Arg1 a1, Arg2 a2) const
			{
				Sentry sentry(*this);

				std::list<Connection>::const_iterator it = Connectable::connections().begin();
				for(; it != _connections.end(); ++it)
				{
					if( &( it->sender() ) != this || false == it->valid() )
						continue;

					const Invokable* invokable = static_cast<const Invokable*>( it->slot().callable() );
					invokable->invoke(a1, a2);
				}
			}

			template <typename Arg1, typename Arg2, typename Arg3>
			inline void send(Arg1 a1, Arg2 a2, Arg3 a3) const
			{
				Sentry sentry(*this);

				std::list<Connection>::const_iterator it = Connectable::connections().begin();
				for(; it != _connections.end(); ++it)
				{
					if( &( it->sender() ) != this || false == it->valid() )
						continue;

					const Invokable* invokable = static_cast<const Invokable*>( it->slot().callable() );
					invokable->invoke(a1, a2, a3);
				}
			}

			inline void operator()() const
			{ this->send(); }

			template <typename Arg1>
			inline void operator()(Arg1 a1) const
			{ this->send(a1); }

			template <typename Arg1, typename Arg2>
			inline void operator()(Arg1 a1, Arg2 a2) const
			{ this->send(a1, a2); }

			template <typename Arg1, typename Arg2, typename Arg3>
			inline void operator()(Arg1 a1, Arg2 a2, Arg3 a3) const
			{ this->send(a1, a2, a3); }

		private:
			mutable bool _sending;
			mutable bool _cleanup;
	};


	template < typename A1 = Pt::Void,
	            typename A2 = Pt::Void,
	            typename A3 = Pt::Void >
	class PT_EXPORT SignalSlot : public BasicSlot<void, A1, A2, A3> {
		public:
			SignalSlot(Pt::Signal<A1, A2, A3>& signal)
			: _method( &signal, &Signal<A1, A2, A3>::send )
			{}

			BasicSlot<void, A1, A2, A3>* clone() const
			{ return new SignalSlot(*this); }

			virtual const void* callable() const
			{
				return &_method;
			}

			virtual void opened(const Connection& c)
			{
				Connectable& connectable = _method.object();
				connectable.opened(c);
			}

			virtual void closed(const Connection& c)
			{
				Connectable& connectable = _method.object();
				connectable.closed(c);
			}

		private:
			mutable ConstMethod<void, Signal<A1, A2, A3>, A1, A2, A3> _method;
	};


	template < typename A1,
	            typename A2,
	            typename A3 >
	SignalSlot<A1, A2, A3> slot( Pt::Signal<A1, A2, A3>& signal )
	{ return SignalSlot<A1, A2, A3>( signal ); }


	template <typename R, typename A1, typename A2, typename A3>
	Connection connect(Signal<A1, A2, A3>& signal, const BasicSlot<R, A1, A2, A3>& slot)
	{
		return Connection(signal, slot.clone() );
	}


	//! Connects a Signal to a member function.
	template <typename R>
	Connection connect(Signal<>& signal, R(*func)())
	{
		return connect( signal, slot(func) );
	}


	template <typename R, typename A1>
	Connection connect(Signal<A1>& signal, R(*func)(A1))
	{
		return connect( signal, slot(func) );
	}


	template <typename R, typename A1, typename A2>
	Connection connect(Signal<A1, A2>& signal, R(*func)(A1, A2))
	{
		return connect( signal, slot(func) );
	}


	template <typename R, typename A1, typename A2, typename A3>
	Connection connect(Signal<A1, A2, A3>& signal, R(*func)(A1, A2, A3))
	{
		return connect( signal, slot(func) );
	}


	//! Connects a Signal to a member function.
	template <typename R, class BaseT, class ClassT>
	Connection connect(Signal<>& signal, BaseT& object, R(ClassT::*memFunc)())
	{
		return connect( signal, slot(&object, memFunc) );
	}


	template <typename R, class BaseT, class ClassT, typename A1>
	Connection connect(Signal<A1>& signal, BaseT& object, R(ClassT::*memFunc)(A1))
	{
		return connect( signal, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1, typename A2>
	Connection connect(Signal<A1, A2>& signal, ClassT& object, R(ClassT::*memFunc)(A1, A2))
	{
		return connect( signal, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1, typename A2, typename A3>
	Connection connect(Signal<A1, A2, A3>& signal, ClassT& object, R(ClassT::*memFunc)(A1, A2, A3))
	{
		return connect( signal, slot(&object, memFunc) );
	}


	//! Connects a Signal to a const member function.
	template <typename R, class ClassT>
	Connection connect(Signal<>& signal, ClassT& object, R(ClassT::*memFunc)() const)
	{
		return connect( signal, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1>
	Connection connect(Signal<A1>& signal, ClassT& object, R(ClassT::*memFunc)(A1) const)
	{
		return connect( signal, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1, typename A2>
	Connection connect(Signal<A1, A2>& signal, ClassT& object, R(ClassT::*memFunc)(A1, A2) const)
	{
		return connect( signal, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1, typename A2, typename A3>
	Connection connect(Signal<A1, A2, A3>& signal, ClassT& object, R(ClassT::*memFunc)(A1, A2, A3) const)
	{
		return connect( signal, slot(&object, memFunc) );
	}


	/// Connects a Signal to another Signal
	template <typename A1, typename A2, typename A3>
	Connection connect(Signal<A1, A2, A3>& sender, Signal<A1, A2, A3>& receiver)
	{
		return connect( sender, slot(receiver) );
	}

} // !namespace Pt

#endif
