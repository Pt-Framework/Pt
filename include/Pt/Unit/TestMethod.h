/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Dürner                      *
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
#ifndef PT_UNIT_TESTMETHOD_H
#define PT_UNIT_TESTMETHOD_H

#include <Pt/Method.h>
#include <Pt/Unit/TestCase.h>

#include <string>


namespace Pt {

namespace Unit {

	template <class FixtureT, typename A1 = Pt::Void>
	class TestMethod : public Test
	                  , public Method<void, FixtureT, A1>
	{
		public:
			typedef void (FixtureT::*MemFuncT)(A1);

		public:
			TestMethod(const std::string& name, FixtureT& fixture, MemFuncT memFunc)
			: Test(name)
			, Method<void, FixtureT, A1>(&fixture, memFunc)
			{}

			void runTest( const std::string& name, const Args& args = Args() )
			{
				this->object().setUp();
				this->call(args);
				this->object().tearDown();
				Test::success( this->name() );
			}
	};


	template <class FixtureT>
	class TestMethod<FixtureT, Pt::Void> : public Test
	                                      , public Method<void, FixtureT>
	{
		public:
			typedef void (FixtureT::*MemFuncT)();

		public:
			TestMethod(const std::string& name, FixtureT& fixture, MemFuncT memFunc)
			: Test(name)
			, Method<void, FixtureT>(&fixture, memFunc)
			{}

			void runTest( const std::string& name, const Args& args = Args() )
			{
				this->object().setUp();
				this->call(args);
				this->object().tearDown();
				Test::success( this->name() );
			}
	};

} // namespace Unit

} // namespace Pt

#endif

