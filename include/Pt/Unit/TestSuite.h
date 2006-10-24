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
#ifndef PT_UNIT_TESTSUITE_H
#define PT_UNIT_TESTSUITE_H

#include <Pt/Exception.h>
#include <Pt/Invokable.h>
#include <Pt/Unit/Test.h>
#include <Pt/Unit/TestCase.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/Reporter.h>

#include <list>
#include <string>


namespace Pt {

namespace Unit {

	class TestSuite : public Test, public TestFixture
	{
		public:
			TestSuite(const std::string& name)
			: Test(name)
			{ }

			virtual ~TestSuite()
			{
				for(std::list<Test*>::iterator it = _tests.begin(); it != _tests.end(); ++it)
					delete *it;

				_tests.clear();
			}

			template <typename FixtureT>
			void registerMethod(FixtureT& fixture, void (FixtureT::*memFunc)(), const std::string& name)
			{
				this->addTest( new TestMethod<FixtureT>(fixture, memFunc, this->name() + "::" + name) );
			}

			void addTest(Test* test)
			{
				connect(test->success, this->success);
				connect(test->assertion, this->assertion);
				connect(test->exception, this->exception);
				connect(test->error, this->error);

				_tests.push_back( test );
			}

			virtual void run()
			{
				std::list<Test*>::iterator it;
				for( it = _tests.begin(); it != _tests.end(); ++it)
				{
					(*it)->run();
				}
			}

		private:
			std::list<Test*> _tests;
	};


} // namespace Unit

} // namespace Pt

#endif // for header

