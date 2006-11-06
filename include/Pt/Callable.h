/***************************************************************************
 *   Copyright (C) 2005 by Dr. Marc Boris Drner                           *
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

#ifndef Pt_Callable_h
#define Pt_Callable_h

#include <Pt/Api.h>
#include <Pt/Invokable.h>
#include <Pt/Exception.h>

#include <typeinfo>


//! \addtogroup Pt
namespace Pt {

	class PT_EXPORT ICallable
	{
		public:
			virtual ~ICallable()
			{}

			virtual size_t argSize() const = 0;

			virtual const char* argName(size_t index) const = 0;

			virtual const std::type_info& argType(size_t index) const = 0;

			virtual void call(const Args& args) const = 0;
	};


	//! \ingroup Pt
	template < typename R,
	            typename A1 = Pt::Void,
	            typename A2 = Pt::Void,
	            typename A3 = Pt::Void,
	            typename A4 = Pt::Void >
	class Callable : public Invokable<A1, A2, A3, A4>, public ICallable {
		public:
			typedef R ReturnT;
			typedef A1 Arg1T;
			typedef A2 Arg2T;
			typedef A3 Arg3T;
			typedef A4 Arg4T;
			enum { NumArgs = 4 };

		public:
			virtual Callable* clone() const = 0;

			Invokable<A1, A2, A3, A4>* cloneInvokable() const
			{ return this->clone(); }

			virtual R operator()(A1, A2, A3, A4) const = 0;

			R call(A1 a1, A2 a2, A3 a3, A4 a4) const
			{ return this->operator()(a1, a2, a3, a4); }

			size_t argSize() const
			{ return NumArgs; }

			const char* argName(size_t index) const
			{
				switch(index)
				{
					case 0: return TypeTraits<A1>::typeName();
					case 1: return TypeTraits<A2>::typeName();
					case 2: return TypeTraits<A3>::typeName();
					case 3: return TypeTraits<A4>::typeName();
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			const std::type_info& argType(size_t index) const
			{
				switch(index)
				{
					case 0: return typeid(A1);
					case 1: return typeid(A2);
					case 2: return typeid(A3);
					case 3: return typeid(A4);
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			void call(const Args& a) const
			{
				const BasicArgs<A1, A2, A3, A4>* args = dynamic_cast< const BasicArgs<A1, A2, A3, A4>* >(&a);
				if(!args)
					throw IllegalArgument("No such argument", PT_SOURCEINFO);

				this->operator()( args->first(), args->second(), args->third(), args->fourth() );
			}

			void invoke(A1 a1, A2 a2, A3 a3, A4 a4) const
			{ this->operator()(a1, a2, a3, a4); }
	};


	template < typename A1,
	            typename A2,
	            typename A3,
	            typename A4 >
	class Callable<void, A1, A2, A3, A4> : public Invokable<A1, A2, A3, A4>, public ICallable {
		public:
			typedef void ReturnT;
			typedef A1 Arg1T;
			typedef A2 Arg2T;
			typedef A3 Arg3T;
			typedef A4 Arg4T;
			enum { NumArgs = 4 };

		public:
			virtual Callable* clone() const = 0;

			Invokable<A1, A2, A3, A4>* cloneInvokable() const
			{ return this->clone(); }

			virtual void operator()(A1, A2, A3, A4) const = 0;

			void call(A1 a1, A2 a2, A3 a3, A4 a4) const
			{ return this->operator()(a1, a2, a3, a4); }

			size_t argSize() const
			{ return NumArgs; }

			const char* argName(size_t index) const
			{
				switch(index)
				{
					case 0: return TypeTraits<A1>::typeName();
					case 1: return TypeTraits<A2>::typeName();
					case 2: return TypeTraits<A3>::typeName();
					case 3: return TypeTraits<A4>::typeName();
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			const std::type_info& argType(size_t index) const
			{
				switch(index)
				{
					case 0: return typeid(A1);
					case 1: return typeid(A2);
					case 2: return typeid(A3);
					case 3: return typeid(A4);
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			void call(const Args& a) const
			{
				const BasicArgs<A1, A2, A3, A4>* args = dynamic_cast< const BasicArgs<A1, A2, A3, A4>* >(&a);
				if(!args)
					throw IllegalArgument("No such argument", PT_SOURCEINFO);

				this->operator()( args->first(), args->second(), args->third(), args->fourth() );
			}

			void invoke(A1 a1, A2 a2, A3 a3, A4 a4) const
			{ this->operator()(a1, a2, a3, a4); }
	};


	template < typename R,
	            typename A1,
	            typename A2,
	            typename A3 >
	class Callable<R, A1, A2, A3> : public Invokable<A1, A2, A3>, public ICallable {
		public:
			typedef R ReturnT;
			typedef A1 Arg1T;
			typedef A2 Arg2T;
			typedef A3 Arg3T;
			enum { NumArgs = 3 };

		public:
			virtual Callable* clone() const = 0;

			Invokable<A1, A2, A3>* cloneInvokable() const
			{ return this->clone(); }

			virtual R operator()(A1, A2, A3) const = 0;

			R call(A1 a1, A2 a2, A3 a3) const
			{ return this->operator()(a1, a2, a3); }

			size_t argSize() const
			{ return NumArgs; }

			const char* argName(size_t index) const
			{
				switch(index)
				{
					case 0: return TypeTraits<A1>::typeName();
					case 1: return TypeTraits<A2>::typeName();
					case 2: return TypeTraits<A3>::typeName();
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			const std::type_info& argType(size_t index) const
			{
				switch(index)
				{
					case 0: return typeid(A1);
					case 1: return typeid(A2);
					case 2: return typeid(A3);
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			void call(const Args& a) const
			{
				const BasicArgs<A1, A2, A3>* args = dynamic_cast< const BasicArgs<A1, A2, A3>* >(&a);
				if(!args)
					throw IllegalArgument("No such argument", PT_SOURCEINFO);

				this->operator()( args->first(), args->second(), args->third() );
			}

			void invoke(A1 a1, A2 a2, A3 a3) const
			{ this->operator()(a1, a2, a3); }
	};


	template < typename A1,
	            typename A2,
	            typename A3 >
	class Callable<void, A1, A2, A3> : public Invokable<A1, A2, A3>, public ICallable {
		public:
			typedef void ReturnT;
			typedef A1 Arg1T;
			typedef A2 Arg2T;
			typedef A3 Arg3T;
			enum { NumArgs = 3 };

		public:
			virtual Callable* clone() const = 0;

			Invokable<A1, A2, A3>* cloneInvokable() const
			{ return this->clone(); }

			virtual void operator()(A1, A2, A3) const = 0;

			void call(A1 a1, A2 a2, A3 a3) const
			{ return this->operator()(a1, a2, a3); }

			size_t argSize() const
			{ return NumArgs; }

			const char* argName(size_t index) const
			{
				switch(index)
				{
					case 0: return TypeTraits<A1>::typeName();
					case 1: return TypeTraits<A2>::typeName();
					case 2: return TypeTraits<A3>::typeName();
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			const std::type_info& argType(size_t index) const
			{
				switch(index)
				{
					case 0: return typeid(A1);
					case 1: return typeid(A2);
					case 2: return typeid(A3);
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			void call(const Args& a) const
			{
				const BasicArgs<A1, A2, A3>* args = dynamic_cast< const BasicArgs<A1, A2, A3>* >(&a);
				if(!args)
					throw IllegalArgument("No such argument", PT_SOURCEINFO);

				this->operator()( args->first(), args->second(), args->third() );
			}

			void invoke(A1 a1, A2 a2, A3 a3) const
			{ this->operator()(a1, a2, a3); }
	};


	template < typename R,
	            typename A1,
	            typename A2 >
	class Callable<R, A1, A2, Pt::Void> : public Invokable<A1, A2, Pt::Void>, public ICallable {
		public:
			typedef R ReturnT;
			typedef A1 Arg1T;
			typedef A2 Arg2T;
			typedef Pt::Void Arg3T;
			enum { NumArgs = 2 };

		public:
			virtual Callable* clone() const = 0;

			Invokable<A1, A2>* cloneInvokable() const
			{ return this->clone(); }

			virtual R operator()(A1, A2) const = 0;

			R call(A1 a1, A2 a2) const
			{ return this->operator()(a1, a2); }

			size_t argSize() const
			{ return NumArgs; }

			const char* argName(size_t index) const
			{
				switch(index)
				{
					case 0: return TypeTraits<A1>::typeName();
					case 1: return TypeTraits<A2>::typeName();
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			const std::type_info& argType(size_t index) const
			{
				switch(index)
				{
					case 0: return typeid(A1);
					case 1: return typeid(A2);
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			void call(const Args& a) const
			{
				const BasicArgs<A1, A2>* args = dynamic_cast< const BasicArgs<A1, A2>* >(&a);
				if(!args)
					throw IllegalArgument("No such argument", PT_SOURCEINFO);

				this->operator()( args->first(), args->second() );
			}

			void invoke(A1 a1, A2 a2) const
			{ this->operator()(a1, a2); }
	};


	template < typename A1,
	           typename A2 >
	class Callable<void, A1, A2, Pt::Void> : public Invokable<A1, A2, Pt::Void>, public ICallable {
		public:
			typedef void ReturnT;
			typedef A1 Arg1T;
			typedef A2 Arg2T;
			typedef Pt::Void Arg3T;
			enum { NumArgs = 2 };

		public:
			virtual Callable* clone() const = 0;

			Invokable<A1, A2>* cloneInvokable() const
			{ return this->clone(); }

			virtual void operator()(A1, A2) const = 0;

			void call(A1 a1, A2 a2) const
			{ return this->operator()(a1, a2); }

			size_t argSize() const
			{ return NumArgs; }

			const char* argName(size_t index) const
			{
				switch(index)
				{
					case 0: return TypeTraits<A1>::typeName();
					case 1: return TypeTraits<A2>::typeName();
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			const std::type_info& argType(size_t index) const
			{
				switch(index)
				{
					case 0: return typeid(A1);
					case 1: return typeid(A2);
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			void call(const Args& a) const
			{
				const BasicArgs<A1, A2>* args = dynamic_cast< const BasicArgs<A1, A2>* >(&a);
				if(!args)
					throw IllegalArgument("No such argument", PT_SOURCEINFO);

				this->operator()( args->first(), args->second() );
			}

			void invoke(A1 a1, A2 a2) const
			{ this->operator()(a1, a2); }
	};


	template < typename R,
	           typename A1 >
	class Callable<R, A1, Pt::Void, Pt::Void> : public Invokable<A1, Pt::Void, Pt::Void>, public ICallable {
		public:
			typedef R ReturnT;
			typedef A1 Arg1T;
			typedef Pt::Void Arg2T;
			typedef Pt::Void Arg3T;
			enum { NumArgs = 1 };

		public:
			virtual Callable* clone() const = 0;

			virtual R operator()(A1) const = 0;

			Invokable<A1>* cloneInvokable() const
			{ return this->clone(); }

			R call(A1 a1) const
			{ return this->operator()(a1); }

			size_t argSize() const
			{ return NumArgs; }

			const char* argName(size_t index) const
			{
				switch(index)
				{
					case 0: return TypeTraits<A1>::typeName();
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			const std::type_info& argType(size_t index) const
			{
				switch(index)
				{
					case 0: return typeid(A1);;
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			void call(const Args& a) const
			{
				const BasicArgs<A1>* args = dynamic_cast< const BasicArgs<A1>* >(&a);
				if(!args)
					throw IllegalArgument("No such argument", PT_SOURCEINFO);

				this->operator()( args->first() );
			}

			void invoke(A1 a1) const
			{ this->operator()(a1); }
	};


	template <typename A1>
	class Callable<void, A1, Pt::Void, Pt::Void> : public Invokable<A1, Pt::Void, Pt::Void>, public ICallable {
		public:
			typedef void ReturnT;
			typedef A1 Arg1T;
			typedef Pt::Void Arg2T;
			typedef Pt::Void Arg3T;
			enum { NumArgs = 1 };

		public:
			virtual Callable* clone() const = 0;

			Invokable<A1>* cloneInvokable() const
			{ return this->clone(); }

			virtual void operator()(A1) const = 0;

			void call(A1 a1) const
			{ return this->operator()(a1); }

			size_t argSize() const
			{ return NumArgs; }

			const char* argName(size_t index) const
			{
				switch(index)
				{
					case 0: return TypeTraits<A1>::typeName();
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			const std::type_info& argType(size_t index) const
			{
				switch(index)
				{
					case 0: return typeid(A1);;
				}

				throw IllegalArgument("No such argument", PT_SOURCEINFO);
			}

			void call(const Args& a) const
			{
				const BasicArgs<A1>* args = dynamic_cast< const BasicArgs<A1>* >(&a);
				if(!args)
					throw IllegalArgument("No such argument", PT_SOURCEINFO);

				this->operator()( args->first() );
			}

			void invoke(A1 a1) const
			{ this->operator()(a1); }
	};


	template <typename R>
	class Callable<R, Pt::Void, Pt::Void, Pt::Void> : public Invokable<Pt::Void, Pt::Void, Pt::Void>, public ICallable {
		public:
			typedef R ReturnT;
			typedef Pt::Void Arg1T;
			typedef Pt::Void Arg2T;
			typedef Pt::Void Arg3T;
			enum { NumArgs = 0 };

		public:
			virtual Callable* clone() const = 0;

			Invokable<>* cloneInvokable() const
			{ return this->clone(); }

			virtual R operator()() const = 0;

			R call() const
			{ return this->operator()(); }

			size_t argSize() const
			{ return NumArgs; }

			const char* argName(size_t index) const
			{ throw IllegalArgument("No such argument", PT_SOURCEINFO); }

			const std::type_info& argType(size_t index) const
			{ throw IllegalArgument("No such argument", PT_SOURCEINFO); }

			void call(const Args& a) const
			{
				const BasicArgs<>* args = dynamic_cast< const BasicArgs<>* >(&a);
				if(!args)
					throw IllegalArgument("No such argument", PT_SOURCEINFO);

				this->operator()();
			}

			void invoke() const
			{ this->operator()(); }
	};


	template <>
	class Callable<void, Pt::Void, Pt::Void, Pt::Void> : public Invokable<Pt::Void, Pt::Void, Pt::Void>, public ICallable {
		public:
			typedef void ReturnT;
			typedef Pt::Void Arg1T;
			typedef Pt::Void Arg2T;
			typedef Pt::Void Arg3T;
			enum { NumArgs = 0 };

		public:
			virtual Callable* clone() const = 0;

			Invokable<>* cloneInvokable() const
			{ return this->clone(); }

			virtual void operator()() const = 0;

			void call() const
			{ this->operator()(); }

			size_t argSize() const
			{ return NumArgs; }

			const char* argName(size_t index) const
			{ throw IllegalArgument("No such argument", PT_SOURCEINFO); }

			const std::type_info& argType(size_t index) const
			{ throw IllegalArgument("No such argument", PT_SOURCEINFO); }

			void call(const Args& a) const
			{
				const BasicArgs<>* args = dynamic_cast< const BasicArgs<>* >(&a);
				if(!args)
					throw IllegalArgument("No such argument", PT_SOURCEINFO);

				this->operator()();
			}

			void invoke() const
			{ this->operator()(); }
	};

} // !namespace Pt


#endif
