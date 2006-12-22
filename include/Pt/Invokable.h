/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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

#include <vector>

#include <Pt/Api.h>
#include <Pt/Void.h>
#include <Pt/Any.h>


//! \addtogroup Pt
namespace Pt {


	template < typename A1 = Pt::Void,
	            typename A2 = Pt::Void,
	            typename A3 = Pt::Void,
	            typename A4 = Pt::Void,
	            typename A5 = Pt::Void >
	class PT_API Invokable {
		public:
			virtual ~Invokable()
			{}

			virtual Invokable* cloneInvokable() const = 0;

			virtual void invoke(A1, A2, A3, A4, A5) const = 0;
	};

	
	template < typename A1,
	           typename A2,
	           typename A3,
	           typename A4 >
	class PT_API Invokable<A1, A2, A3, A4, Pt::Void> {
		public:
			virtual ~Invokable()
			{}

			virtual Invokable* cloneInvokable() const = 0;

			virtual void invoke(A1, A2, A3, A4) const = 0;
	};


	template < typename A1,
	            typename A2,
	            typename A3 >
	class PT_API Invokable<A1, A2, A3, Pt::Void, Pt::Void> {
		public:
			virtual ~Invokable()
			{}

			virtual Invokable* cloneInvokable() const = 0;

			virtual void invoke(A1, A2, A3) const = 0;
	};


	template < typename A1,
	            typename A2 >
	class PT_API Invokable<A1, A2, Pt::Void, Pt::Void, Pt::Void> {
		public:
			virtual ~Invokable()
			{}

			virtual Invokable* cloneInvokable() const = 0;

			virtual void invoke(A1, A2) const = 0;
	};


	template <typename A1>
	class PT_API Invokable<A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void> {
		public:
			virtual ~Invokable()
			{}

			virtual Invokable* cloneInvokable() const = 0;

			virtual void invoke(A1) const = 0;
	};


	template <>
	class PT_API Invokable<Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> {
		public:
			virtual ~Invokable()
			{}

			virtual Invokable* cloneInvokable() const = 0;

			virtual void invoke() const = 0;
	};

} // !namespace Pt


#endif
