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
#ifndef PT_UNIT_TESTCASE_H
#define PT_UNIT_TESTCASE_H

#include <Pt/Exception.h>
#include <Pt/Invokable.h>
#include <Pt/Unit/Test.h>
#include <Pt/Unit/TestFixture.h>

#include <list>
#include <string>


namespace Pt {

namespace Unit {

	class TestCase : public Test, public TestFixture
	{
		public:
			TestCase(const std::string& name)
			: Test(name)
			{ }

			virtual ~TestCase()
			{ }

			virtual void run()
			{
				try
				{
					this->setUp();
					this->test();
					this->tearDown();
					Test::success( this->name() );
				}
				catch(const Assertion& assertion)
				{
					Test::assertion( this->name(), assertion );
				}
				catch(const std::exception& ex)
				{
					Test::exception( this->name(), ex );
				}
				catch(...)
				{
					Test::error( this->name() );
				}
			}

			virtual void test() = 0;
	};


	template <class FixtureT>
	class TestMethod : public TestCase
	{
		public:
			typedef void (FixtureT::*MemFuncT)();

		public:
			TestMethod(FixtureT& fixture, MemFuncT memFunc, const std::string& name)
			: TestCase(name)
			, _method(&fixture, memFunc)
			{}

			void setUp()
			{ _method.object().setUp(); }

			void tearDown()
			{ _method.object().tearDown(); }

			void test()
			{ _method.invoke(); }

		private:
			Method<void, FixtureT> _method;
	};

} // namespace Unit

} // namespace Pt

#endif // for header

