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
#include<Pt/Unit/Reporter.h>
#include<Pt/Unit/TestSuite.h>

#include <cstring>


namespace Pt {

namespace Unit {

	class Application
	{
		template <typename TestT>
		friend class RegisterTest;

		public:
			Application()
			{}

			~Application()
			{}

			static void addTest(Test& test)
			{
				connect(test.success, &Application::success);
				connect(test.assertion, &Application::assertion);
				connect(test.exception, &Application::exception);
				connect(test.error, &Application::error);
				_allTests.push_back(&test);
			}

			int run(Reporter& reporter)
			{
				Application::_reporter = &reporter;
			
				_errors = 0;
				_numTests = 0;

				std::list<Test*>::iterator it;
				for(it = _allTests.begin(); it != _allTests.end(); ++it)
				{
					(*it)->run();
				}

				return _errors;
			}

			static void success(const std::string& testName)
			{
				if(_reporter)
				{
					_reporter->success(testName);
				}
			}

			static void assertion(const std::string& testName, const Assertion& a)
			{
				++_errors;

				if(_reporter)
				{
					_reporter->assertion(testName, a);
				}
			}

			static void exception(const std::string& testName, const std::exception& ex)
			{
				++_errors;

				if(_reporter)
				{
					_reporter->exception(testName, ex);
				}
			}

			static void error(const std::string& testName)
			{
				++_errors;

				if(_reporter)
				{
					_reporter->error(testName);
				}
			}

			static void message( const std::string msg )
			{
				if(_reporter)
					_reporter->message(msg);
			}

		private:
			static size_t _errors;

			static size_t _numTests;

			static std::list<Test*> _allTests;

			static Reporter* _reporter;
	};

	size_t Application::_errors = 0;

	size_t Application::_numTests = 0;

	std::list<Test*> Application::_allTests;

	Reporter* Application::_reporter = 0;


	template <class TestT>
	struct RegisterTest
	{
		RegisterTest()
		{
			static TestT test;
			Application::addTest(test);
		}
	};

} // namespace Unit

} // namespace Pt


int main(int argc, char** argv)
{
  // CppUnit(mini) test launcher
  // command line option syntax:
  // test [OPTIONS]
  // where OPTIONS are
  //  -t=CLASS[::TEST]    run the test class CLASS or member test CLASS::TEST
  //  -x=CLASS[::TEST]    run all except the test class CLASS or member test CLASS::TEST
  //  -f=FILE             save output in file FILE instead of stdout

/*
	char* fileName = 0;
	char* testName = "";
	char* xtestName = "";
	
	for(int i = 1; i < argc; ++i)
	{
		if(argv[i][0] != '-')
			break;

		if( !strncmp(argv[i], "-t=", 3) ) 
		{
			testName = argv[i] + 3;
		}
		else if( !strncmp(argv[i], "-f=", 3) ) 
		{
			fileName = argv[i] + 3;
		}
		else if ( !strncmp(argv[i], "-x=", 3) ) 
		{
			xtestName = argv[i] + 3;
		}
	}
*/
	Pt::Unit::Application app;
	Pt::Unit::Reporter reporter;

	return app.run(reporter);
}
