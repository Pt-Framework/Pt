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

    class Application
    {
        template <typename TestT>
        friend struct RegisterTest;

        public:
            Application()
            {}

            virtual ~Application()
            {}

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

            const std::list<Test*>& tests() const
            { return _allTests; }

            static void started(const Test& test)
            {
                if(_reporter)
                {
                    _reporter->started(test);
                }
            }

            static void finished(const Test& test)
            {
                if(_reporter)
                {
                    _reporter->finished(test);
                }
            }

            static void success(const Test& test)
            {
                if(_reporter)
                {
                    _reporter->success(test);
                }
            }

            static void assertion(const Test& test, const Assertion& a)
            {
                ++_errors;

                if(_reporter)
                {
                    _reporter->assertion(test, a);
                }
            }

            static void exception(const Test& test, const std::exception& ex)
            {
                ++_errors;

                if(_reporter)
                {
                    _reporter->exception(test, ex);
                }
            }

            static void error(const Test& test)
            {
                ++_errors;

                if(_reporter)
                {
                    _reporter->error(test);
                }
            }

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
