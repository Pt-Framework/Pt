/***************************************************************************
 *   Copyright (C) 2004-2006 by Marc Boris Dürner                          *
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

#ifndef Pt_Delegate_h
#define Pt_Delegate_h

#include <Pt/Void.h>
#include <Pt/Function.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/Connectable.h>
#include <Pt/Exception.h>

#include <iostream>


namespace Pt {

	/** A Delegate can be connected to one target, but forwards the return
	    value of the target, when called. **/
	template <typename R, typename A1 = Pt::Void, typename A2 = Pt::Void, typename A3 = Pt::Void>
	class PT_API Delegate : public Connectable {
		public:
			typedef Callable<R, A1, A2, A3> Callable;

		public:
			Delegate()
			: _targetConnection(0)
			{ }

			Delegate(const Delegate& rhs)
			: _targetConnection(0)
			{ Delegate::operator=(rhs); }

			~Delegate()
			{
				Delegate::disconnect();
			}

			Delegate& operator=(const Delegate& other)
			{
				Connectable::operator=(other);

				Delegate::disconnect();

				if( other._targetConnection )
				{
					const Slot& slot = other._targetConnection->slot();
					Connection connection( *this, slot.clone()  );
				}

				return *this;
			}

			virtual void opened(const Connection& c)
			{
				if( this == &c.sender() )
				{
					this->disconnect();
					_targetConnection = new Connection(c);
				}
				else
				{
					Connectable::opened(c);
				}
			}

			virtual void closed(const Connection& c)
			{
				if( this == &c.sender() )
				{
					this->disconnect();
				}
				else
				{
					Connectable::closed(c);
				}
			}

			void disconnect()
			{
				delete _targetConnection;
				 _targetConnection = 0;
			}

			inline R call() const
			{
				if(!_targetConnection) {
					throw Exception("Empty BasicDelegate called.", PT_SOURCEINFO);
				}
				const Callable* cb = static_cast<const Callable*>( _targetConnection->slot().callable() );
				return cb->call();
			}


			template <typename Arg1>
			inline R call(Arg1 a1) const
			{
				if(!_targetConnection) {
					throw Exception("Empty BasicDelegate called.", PT_SOURCEINFO);
				}
				const Callable* cb = static_cast<const Callable*>( _targetConnection->slot().callable() );
				return cb->call(a1);
			}

			template <typename Arg1, typename Arg2>
			inline R call(Arg1 a1, Arg2 a2) const
			{
				if(!_targetConnection) {
					throw Exception("Empty BasicDelegate called.", PT_SOURCEINFO);
				}
				const Callable* cb = static_cast<const Callable*>( _targetConnection->slot().callable() );
				return cb->call(a1, a2);
			}

			template <typename Arg1, typename Arg2, typename Arg3>
			inline R call(Arg1 a1, Arg2 a2, Arg3 a3) const
			{
				if(!_targetConnection) {
					throw Exception("Empty BasicDelegate called.", PT_SOURCEINFO);
				}
				const Callable* cb = static_cast<const Callable*>( _targetConnection->slot().callable() );
				return cb->call(a1, a2, a3);
			}

			inline void invoke() const
			{
				if(!_targetConnection) {
					return;
				}
				const Callable* cb = static_cast<const Callable*>( _targetConnection->slot().callable() );
				cb->call();
			}

			template <typename Arg1>
			inline void invoke(Arg1 a1) const
			{
				if(!_targetConnection) {
					return;
				}
				const Callable* cb = static_cast<const Callable*>( _targetConnection->slot().callable() );
				cb->call(a1);
			}

			template <typename Arg1, typename Arg2>
			inline void invoke(Arg1 a1, Arg2 a2) const
			{
				if(!_targetConnection) {
					return;
				}
				const Callable* cb = static_cast<const Callable*>( _targetConnection->slot().callable() );
				cb->call(a1, a2);
			}

			template <typename Arg1, typename Arg2, typename Arg3>
			inline void invoke(Arg1 a1, Arg2 a2, Arg3 a3)
			{
				if(!_targetConnection) {
					return;
				}
				const Callable* cb = static_cast<const Callable*>( _targetConnection->slot().callable() );
				cb->call(a1, a2, a3);
			}

			R operator()() const
			{ return this->call(); }

			template <typename Arg1>
			R operator()(Arg1 a1) const
			{ return this->call(a1); }

			template <typename Arg1, typename Arg2>
			R operator()(Arg1 a1, Arg2 a2) const
			{ return this->call(a1, a2); }

			template <typename Arg1, typename Arg2, typename Arg3>
			R operator()(Arg1 a1, Arg2 a2, Arg3 a3) const
			{ return this->call(a1,a2 ,a3); }

		private:
			Connection* _targetConnection;
	};




	template <typename R, typename A1, typename A2, typename A3>
	Connection connect(Delegate<R, A1, A2, A3>& delegate, const Function<R, A1, A2, A3>& func)
	{
		Function<R, A1, A2, A3>* function = func.clone();
		delegate.attach(*function);
		return Connection(delegate, function);
	}


	template <typename R>
	Connection connect(Delegate<R>& delegate, R(*func)())
	{
		return connect( delegate, slot(func) );
	}


	template <typename R, typename A1>
	Connection connect(Delegate<R, A1>& delegate, R(*func)(A1))
	{
		return connect( delegate, slot(func) );
	}


	template <typename R, typename A1, typename A2>
	Connection connect(Delegate<R, A1, A2>& delegate, R(*func)(A1, A2))
	{
		return connect( delegate, slot(func) );
	}


	template <typename R, typename A1, typename A2, typename A3>
	Connection connect(Delegate<R, A1, A2, A3>& delegate, R(*func)(A1, A2, A3))
	{
		return connect( delegate, slot(func) );
	}




	template <typename R, typename A1, typename A2, typename A3>
	void disconnect(Delegate<R, A1, A2, A3>& delegate, const Function<R, A1, A2, A3>& func)
	{
		if( const Connection* connection = delegate.connection(func) ) {
			Connection tmp = *connection;
			tmp.close();
		}
	}


	template <typename R>
	void disconnect(Delegate<R>& delegate, R(*func)())
	{
		disconnect( delegate, slot(func) );
	}


	template <typename R, typename A1>
	void disconnect(Delegate<R, A1>& delegate, R(*func)(A1))
	{
		disconnect( delegate, slot(func) );
	}


	template <typename R, typename A1, typename A2>
	void disconnect(Delegate<R, A1, A2>& delegate, R(*func)(A1, A2))
	{
		disconnect( delegate, slot(func) );
	}


	template <typename R, typename A1, typename A2, typename A3>
	void disconnect(Delegate<R, A1, A2, A3>& delegate, R(*func)(A1, A2, A3))
	{
		disconnect( delegate, slot(func) );
	}




	template <typename R, class ClassT, typename A1, typename A2, typename A3>
	Connection connect(Delegate<R, A1, A2, A3>& delegate, const Method<R, ClassT, A1, A2, A3>& m)
	{
		Method<R, ClassT, A1, A2, A3>* method = m.clone();
		delegate.attach(*method);
		return Connection(delegate, method->object(), method);
	}


	template <typename R, class ClassT>
	Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)())
	{
		return connect( delegate, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1>
	Connection connect(Delegate<R, A1>& delegate, ClassT& object, R(ClassT::*memFunc)(A1))
	{
		return connect( delegate, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1, typename A2>
	Connection connect(Delegate<R, A1, A2>& delegate, ClassT& object, R(ClassT::*memFunc)(A1, A2))
	{
		return connect( delegate, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1, typename A2, typename A3>
	Connection connect(Delegate<R, A1, A2, A3>& delegate, ClassT& object, R(ClassT::*memFunc)(A1, A2, A3))
	{
		return connect( delegate, slot(&object, memFunc) );
	}




	//! Disconnects a Signal from a member function.
	template <typename R, class ClassT, typename A1, typename A2, typename A3>
	void disconnect(Delegate<R, A1, A2, A3>& delegate, const Method<R, ClassT, A1, A2, A3>& method )
	{
		if( const Connection* connection = delegate.connection( method ) ) {
			Connection tmp = *connection;
			tmp.close();
		}
	}


	template <typename R, class ClassT>
	void disconnect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)())
	{
		disconnect( delegate, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1>
	void disconnect(Delegate<R, A1>& delegate, ClassT& object, R(ClassT::*memFunc)(A1))
	{
		disconnect( delegate, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1, typename A2>
	void disconnect(Delegate<R, A1, A2>& delegate, ClassT& object, R(ClassT::*memFunc)(A1, A2))
	{
		disconnect( delegate, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1, typename A2, typename A3>
	void disconnect(Delegate<R, A1, A2, A3>& delegate, ClassT& object, R(ClassT::*memFunc)(A1, A2, A3))
	{
		disconnect( delegate, slot(&object, memFunc) );
	}




	template <typename R, class ClassT, typename A1, typename A2, typename A3>
	Connection connect(Delegate<R, A1, A2, A3>& delegate, const ConstMethod<R, ClassT, A1, A2, A3>& m)
	{
		ConstMethod<R, ClassT, A1, A2, A3>* method = m.clone();
		delegate.attach(*method);
		return Connection(delegate, method->object(), method);
	}


	template <typename R, class ClassT>
	Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)() const)
	{
		return connect( delegate, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1>
	Connection connect(Delegate<R, A1>& delegate, ClassT& object, R(ClassT::*memFunc)(A1) const)
	{
		return connect( delegate, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1, typename A2>
	Connection connect(Delegate<R, A1, A2>& delegate, ClassT& object, R(ClassT::*memFunc)(A1, A2) const)
	{
		return connect( delegate, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1, typename A2, typename A3>
	Connection connect(Delegate<R, A1, A2, A3>& delegate, ClassT& object, R(ClassT::*memFunc)(A1, A2, A3) const)
	{
		return connect( delegate, slot(&object, memFunc) );
	}




	//! Disconnects a Delegate from a const member function.
	template <typename R, class ClassT, typename A1, typename A2, typename A3>
	void disconnect(Delegate<R, A1, A2, A3>& delegate, const ConstMethod<R, ClassT, A1, A2, A3>& method )
	{
		if( const Connection* connection = delegate.connection( method ) ) {
			Connection tmp = *connection;
			tmp.close();
		}
	}


	template <typename R, class ClassT>
	void disconnect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)() const)
	{
		disconnect( delegate, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1>
	void disconnect(Delegate<R, A1>& delegate, ClassT& object, R(ClassT::*memFunc)(A1) const)
	{
		disconnect( delegate, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1, typename A2>
	void disconnect(Delegate<R, A1, A2>& delegate, ClassT& object, R(ClassT::*memFunc)(A1, A2) const)
	{
		disconnect( delegate, slot(&object, memFunc) );
	}


	template <typename R, class ClassT, typename A1, typename A2, typename A3>
	void disconnect(Delegate<R, A1, A2, A3>& delegate, ClassT& object, R(ClassT::*memFunc)(A1, A2, A3) const)
	{
		disconnect( delegate, slot(&object, memFunc) );
	}
} // !namespace Pt

#endif
