/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Duerner                     *
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
#ifndef PT_UNIT_APPLICATION_H
#define PT_UNIT_APPLICATION_H

#include<Pt/Unit/Reporter.h>
#include<Pt/Unit/Test.h>

#include <sstream>


namespace Pt {

namespace Unit {

    /** @brief Run registered tests

        The application class serves as a container for a number of tests to
        be run. Test can be registered easily with the RegisterTest<> class
        template.

        @code
            class MyTest : public Unit::TestCase
            { ... };

            RegisterTest<MyTest> _registerMyTest;
        @endcode

        The application uses a reporter to process test events. Reporters can
        be made to print information to the console or write XML logs.
    */
    class Application
    {
        template <typename TestT>
        friend struct RegisterTest;

        public:
            /** @brief Default Constructor
            */
            Application()
            {}

            /** @brief Destructor
            */
            virtual ~Application()
            {}

            /** @brief Set Reporter for test events

                Sets a new reporter to report test events. Caller owns the
                reporter and must make sure it lives as long as the 
                application.

                @param reporter 
            */
            void setReporter(Reporter& reporter)
            { Application::_reporter = &reporter; }

            static void registerTest(Test& test)
            {
                connect(test.started, &Application::started);
                connect(test.finished, &Application::finished);
                connect(test.success, &Application::success);
                connect(test.assertion, &Application::assertion);
                connect(test.exception, &Application::exception);
                connect(test.error, &Application::error);
                _allTests.push_back(&test);
            }

            /** @brief Run all tests

                This method will run all tests that have been regietered 
                previously. Use the RegisterTest<T> template to register
                a test to the application. The function will return the
                number of failed tests, so that its return value can directly
                be used to return from the main function.

                @return Number of failed tests.
            */
            int run()
            {
                _errors = 0;

                std::list<Test*>::iterator it;
                for(it = _allTests.begin(); it != _allTests.end(); ++it)
                {
                        (*it)->run();
                }

                return _errors;
            }

            /** @brief Run test by name

                This method will a previously registered tests. Use the
                RegisterTest<T> template to register a test to the application.
                The function will return the number of failed tests, so that 
                its return value can directly be used to return from the main 
                function.

                @param testName name of the test to be run
                @return Number of failed tests.
            */
            int run(const std::string& testName = "")
            {
                _errors = 0;

                std::list<Test*>::iterator it;
                for(it = _allTests.begin(); it != _allTests.end(); ++it)
                {
                    if(testName == "" || (*it)->name() == testName)
                        (*it)->run();
                }

                return _errors;
            }

            /** @brief Returns a list of all registered test

                @return Reference to the registered tests.
            */
            const std::list<Test*>& tests() const
            { return _allTests; }

            /** @brief Process started event
            */
            static void started(const Test& test)
            {
                if(_reporter)
                {
                    _reporter->started(test);
                }
            }

            /** @brief Process finished event
            */
            static void finished(const Test& test)
            {
                if(_reporter)
                {
                    _reporter->finished(test);
                }
            }

            /** @brief Process success event
            */
            static void success(const Test& test)
            {
                if(_reporter)
                {
                    _reporter->success(test);
                }
            }

            /** @brief Process assertion event
            */
            static void assertion(const Test& test, const Assertion& a)
            {
                ++_errors;

                if(_reporter)
                {
                    _reporter->assertion(test, a);
                }
            }

            /** @brief Process exception event
            */
            static void exception(const Test& test, const std::exception& ex)
            {
                ++_errors;

                if(_reporter)
                {
                    _reporter->exception(test, ex);
                }
            }

            /** @brief Process error event
            */
            static void error(const Test& test)
            {
                ++_errors;

                if(_reporter)
                {
                    _reporter->error(test);
                }
            }


            /** @brief Process informational messages
            */
            static void message( const std::string msg )
            {
                if(_reporter)
                    _reporter->message(msg);
            }

        private:
            static size_t _errors;

            static std::list<Test*> _allTests;

            static Reporter* _reporter;
    };

    size_t Application::_errors = 0;

    std::list<Test*> Application::_allTests;

    Reporter* Application::_reporter = 0;


    template <class TestT>
    struct RegisterTest
    {
        RegisterTest()
        {
            static TestT test;
            Application::registerTest(test);
        }
    };

} // namespace Unit

} // namespace Pt

#endif
