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

#include <Pt/Reflectable.h>

#include <Pt/Unit/Test.h>
#include <Pt/Unit/TestFixture.h>

#include <list>
#include <map>
#include <string>


namespace Pt {

namespace Unit {

	class TestSuite : public Test, public TestFixture
	{
		public:
			class DefaultProtocol : public TestProtocol
			{
				public:
					void run(Test& test)
					{
						TestSuite& suite = static_cast<TestSuite&>(test);
						const MethodMap& methods = suite.methods();
						MethodMap::const_iterator it;
						for(it = methods.begin(); it != methods.end(); ++it)
						{
							suite.runTest( it->first, BasicArgs<>() );
						}
					}
			};

		public:
			TestSuite(const std::string& name,
			          TestProtocol& protocol = TestSuite::Default)
			: Reflectable(name)
			, Test(name)
			{
				this->setProtocol(protocol);
			}

			void runTest( const std::string& name, const Args& args = Args() )
			{
				this->setUp();
				Reflectable::call(name, args);
				this->tearDown();

				Test::success( this->name() + "::" + name );
			}

			static DefaultProtocol Default;
	};

	TestSuite::DefaultProtocol TestSuite::Default;




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

			void run(Test& suite)
			{
				std::multimap<std::string, Args*>::iterator it;
				for(it = _items.begin(); it != _items.end(); ++it)
				{
					suite.runTest( it->first, *it->second );
				}
			}

		private:
			std::multimap<std::string, Args*> _items;
	};

} // namespace Unit

} // namespace Pt

#endif // for header

