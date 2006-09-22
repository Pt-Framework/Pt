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
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/Reporter.h>

#include <list>
#include <string>


namespace Pt {

namespace Unit {

	class TestCase : public TestFixture
	{
		public:
			TestCase(const std::string& name)
			: _name(name)
			{
				_testCases.push_back(this);
			}

			virtual ~TestCase()
			{
				for(std::list<Test*>::iterator it = _tests.begin(); it != _tests.end(); ++it)
					delete *it;

				_tests.clear();
			}

			template <typename InvokableT>
			void registerTest(const InvokableT& invokable, const std::string& name)
			{
				_tests.push_back( new Test(invokable, name) );
			}

			const std::string& name() const
			{ return _name; }

			size_t errors() const
			{ return _errors; }

			void run()
			{
				std::list<Test*>::iterator it;
				for( it = _tests.begin(); it != _tests.end(); ++it)
				{
					this->setUp();

					_numTests++;

					try
					{
						(*it)->run();
						this->success( this->name(), (*it)->name() );
					}
					catch(const Assertion& assertion)
					{
						this->assertion( this->name(), (*it)->name(), assertion.sourceInfo() );
					}
					catch(const std::exception& ex)
					{
						this->exception( this->name(), (*it)->name(), ex.what() );
					}
					catch(...)
					{
						this->error( this->name(), (*it)->name() );
					}

					this->tearDown();
				}
			}

			void success(const std::string className, const std::string functionName)
			{
				if(_reporter)
				{
					_reporter->success(className, functionName);
				}
			}

			void assertion(const std::string className, const std::string functionName, const SourceInfo& info)
			{
				++_errors;

				if(_reporter)
				{
					_reporter->assertion(className, functionName, info);
				}
			}

			void exception(const std::string className, const std::string functionName, const std::string what)
			{
				++_errors;

				if(_reporter)
				{
					_reporter->exception(className, functionName, what);
				}
			}

			void error(const std::string className, const std::string functionName)
			{
				++_errors;

				if(_reporter)
				{
					_reporter->error(className, functionName);
				}
			}

			static void message( const std::string msg )
			{
				if(_reporter)
					_reporter->message(msg);
			}

			static int start(Reporter& reporter)
			{
				TestCase::_reporter = &reporter;
			
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

		private:
			std::string _name;
			std::list<Test*> _tests;


		private:
			static size_t _errors;

			static size_t _numTests;

			static std::list<TestCase*> _testCases;

			static Reporter* _reporter;
	};

	size_t Pt::Unit::TestCase::_errors = 0;

	size_t Pt::Unit::TestCase::_numTests = 0;

	std::list<Pt::Unit::TestCase*> Pt::Unit::TestCase::_testCases;

	Reporter* Pt::Unit::TestCase::_reporter = 0;

} // namespace Unit

} // namespace Pt

#endif // for header

