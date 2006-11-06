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

#include <Pt/Unit/Test.h>
#include <Pt/Unit/TestFixture.h>
#include <Pt/Unit/TestFunction.h>
#include <Pt/Unit/TestMethod.h>

#include <list>
#include <string>


namespace Pt {

namespace Unit {

	class TestSuite : public Test, public TestFixture
	{
		public:
			TestSuite(const std::string& name, TestProtocol& protocol = defaultProtocol)
			: Test(name)
			{ this->setProtocol(protocol); }

			virtual ~TestSuite()
			{
				for(std::list<Test*>::iterator it = _tests.begin(); it != _tests.end(); ++it)
				{
					delete *it;
				}

				_tests.clear();
			}

			template <typename FixtureT>
			void registerMethod(FixtureT& fixture, void (FixtureT::*memFunc)(), const std::string& name)
			{
				this->registerTest( new TestMethod<FixtureT>(this->name() + "::" + name, fixture, memFunc) );
			}

			template <class FixtureT, typename A1>
			void registerMethod(FixtureT& fixture, void (FixtureT::*memFunc)(A1), const std::string& name)
			{
				this->registerTest( new TestMethod<FixtureT, A1>(this->name() + "::" + name, fixture, memFunc) );
			}

			void registerTest(Test* test)
			{
				connect(test->success, this->success);
				connect(test->assertion, this->assertion);
				connect(test->exception, this->exception);
				connect(test->error, this->error);
				_tests.push_back( test );
			}

			void runTest(const std::string& name, const Args& args = Args())
			{
				std::list<Test*>::const_iterator it;
				for( it = _tests.begin(); it != _tests.end(); ++it)
				{
					if( (*it)->name() == name ) {
						(*it)->runTest(name, args);
						return;
					}
				}
			}

		private:
			std::list<Test*> _tests;
	};


	class TestSuiteProtocol : public TestProtocol
	{
		public:
			void includeTest(const std::string& testName)
			{
				_items.insert( std::make_pair(testName, new BasicArgs<>()) );
			}

			template <typename A1>
			void includeTest(const std::string& testName, A1 a1)
			{
				_items.insert( std::make_pair(testName, new BasicArgs<A1>(a1)) );
			}

			void run(Test& test)
			{
				std::map<std::string, Args*>::iterator it;
				for(it = _items.begin(); it != _items.end(); ++it)
				{
					test.runTest( test.name() + "::" + it->first, *it->second );
				}
			}

		private:
			std::multimap<std::string, Args*> _items;
	};

} // namespace Unit

} // namespace Pt

#endif // for header

