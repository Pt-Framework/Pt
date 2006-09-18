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

#include <Pt/Unit/TestFixture.h>
#include <Pt/Invokable.h>

#include <list>
#include <string>
#include <iostream>


namespace Pt {

namespace Unit {

	class TestCase : public TestFixture
	{
		public:
			TestCase()
			{
				_testCases.push_back(this);
			}

			virtual ~TestCase()
			{
				std::list< Pt::Invokable<>* >::iterator it;
				for( it = _tests.begin(); it != _tests.end(); ++it)
				{
					delete *it;
				}
				_tests.clear();
			}

			template <typename I>
			void registerTest(const I& invokable)
			{
				_tests.push_back( invokable.clone() );
			}

			int errors() const
			{ return _errors; }

			void run()
			{
				std::list< Pt::Invokable<>* >::iterator it;
				for( it = _tests.begin(); it != _tests.end(); ++it)
				{
					try {
						this->setUp();
						//this->progress("AtomicTest", "test");
						(*it)->invoke();
						this->tearDown();
					}
					catch(...) {
						TestCase::error("Test Failed: An Exception was thrown.", "test", __FILE__, __LINE__);
					}
				}
			}

			void assertion(bool cond)
			{
				if(!cond)
				{
					TestCase::error("ASSERTION", "...", __FILE__, __LINE__);
				}
			}

			void error(const char* macroName, const char* macro, const char* file, int line)
			{
				_errors++;

				if(_out)
					*_out << macroName << ": line " << line << " Error: " << macro << std::endl;
			}

			static void message( const char *msg )
			{
				if(_out)
					*_out << msg;
			}


			void progress(const char* className, const char* functionName)
			{
				_numTests++;

				if(_out)
					*_out << className << "::" << functionName << " : OK"<< std::endl;
			}

			static int start(std::ostream& out)
			{
				TestCase::_out = &out;
			
				_errors = 0;
				_numTests = 0;

				std::list<TestCase*>::iterator it;
				for(it = _testCases.begin(); it != _testCases.end(); ++it)
				{
					TestCase* tc = *it;
					tc->run();
				}

				return _errors;
			}

		protected:
			static int _errors;

			static int _numTests;

			static std::list<TestCase*> _testCases;
		
			static std::ostream* _out;

		private:
			std::list< Pt::Invokable<>* > _tests;
	};

	int Pt::Unit::TestCase::_errors = 0;

	int Pt::Unit::TestCase::_numTests = 0;

	std::list<Pt::Unit::TestCase*> Pt::Unit::TestCase::_testCases;

	std::ostream* Pt::Unit::TestCase::_out = 0;

} // namespace Unit

} // namespace Pt

#endif // for header

