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

#ifndef Pt_Invokable_h
#define Pt_Invokable_h

#include <Pt/Api.h>
#include <Pt/Void.h>


//! \addtogroup Pt
namespace Pt {

	class Args
	{
		public:
			virtual ~Args()
			{}
	};


	template < typename A1 = Pt::Void,
	            typename A2 = Pt::Void,
	            typename A3 = Pt::Void,
	            typename A4 = Pt::Void >
	class BasicArgs : public Args
	{
		public:
			BasicArgs(A1 first, A2 second, A3 third, A4 fourth)
			: _first(first)
			, _second(second)
			, _third(third)
			, _fourth(fourth)
			{}

			A1 first() const
			{ return _first; }

			A2 second() const
			{ return _second; }

			A3 third() const
			{ return _third; }

			A4 fourth() const
			{ return _fourth; }

		private:
			A1 _first;
			A2 _second;
			A3 _third;
			A4 _fourth;
	};


	template < typename A1,
	            typename A2,
	            typename A3 >
	class BasicArgs<A1, A2, A3, Pt::Void> : public Args
	{
		public:
			BasicArgs(A1 first, A2 second, A3 third)
			: _first(first)
			, _second(second)
			, _third(third)
			{}

			A1 first() const
			{ return _first; }

			A2 second() const
			{ return _second; }

			A3 third() const
			{ return _third; }

		private:
			A1 _first;
			A2 _second;
			A3 _third;
	};

	template < typename A1,
	            typename A2 >
	class BasicArgs<A1, A2, Pt::Void, Pt::Void> : public Args
	{
		public:
			BasicArgs(A1 first, A2 second)
			: _first(first)
			, _second(second)
			{}

			A1 first() const
			{ return _first; }

			A2 second() const
			{ return _second; }

		private:
			A1 _first;
			A2 _second;
	};


	template < typename A1 >
	class BasicArgs<A1, Pt::Void, Pt::Void, Pt::Void> : public Args
	{
		public:
			BasicArgs(A1 first)
			: _first(first)
			{}

			A1 first() const
			{ return _first; }

		private:
			A1 _first;
	};


	template < >
	class BasicArgs<Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public Args
	{
		public:
			BasicArgs()
			{}
	};


	//! \ingroup Pt
	template < typename A1 = Pt::Void,
	            typename A2 = Pt::Void,
	            typename A3 = Pt::Void,
	            typename A4 = Pt::Void >
	class PT_EXPORT Invokable {
		public:
			virtual ~Invokable()
			{}

			virtual Invokable* cloneInvokable() const = 0;

			virtual void invoke(A1, A2, A3, A4) const = 0;
	};


	template < typename A1,
	            typename A2,
	            typename A3 >
	class PT_EXPORT Invokable<A1, A2, A3> {
		public:
			virtual ~Invokable()
			{}

			virtual Invokable* cloneInvokable() const = 0;

			virtual void invoke(A1, A2, A3) const = 0;
	};


	template < typename A1,
	            typename A2 >
	class PT_EXPORT Invokable<A1, A2, Pt::Void> {
		public:
			virtual ~Invokable()
			{}

			virtual Invokable* cloneInvokable() const = 0;

			virtual void invoke(A1, A2) const = 0;
	};


	template <typename A1>
	class PT_EXPORT Invokable<A1, Pt::Void, Pt::Void> {
		public:
			virtual ~Invokable()
			{}

			virtual Invokable* cloneInvokable() const = 0;

			virtual void invoke(A1) const = 0;
	};


	template <>
	class PT_EXPORT Invokable<Pt::Void, Pt::Void, Pt::Void> {
		public:
			virtual ~Invokable()
			{}

			virtual Invokable* cloneInvokable() const = 0;

			virtual void invoke() const = 0;
	};

} // !namespace Pt


#endif
