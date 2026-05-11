/*
 * Copyright (C) 2005-2008 by Dr. Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef PT_UNIT_APPLICATION_H
#define PT_UNIT_APPLICATION_H

#include <Pt/Unit/Api.h>
#include <Pt/Unit/Reporter.h>
#include <Pt/Unit/Test.h>
#include <sstream>

namespace Pt {

namespace Unit {

    /** @brief Run registered tests

        The %Application class serves as the environment for running unit
        tests. Tests are registered at program start using
        %Pt::Unit::RegisterTest. A %Reporter can be attached to process
        test events such as printing results to the console or writing
        log files.

        The simplest way to provide a %main() function is to include
        TestMain.h in exactly one source file of the test executable.
        It creates an %Application, attaches a %BriefReporter for console
        output and supports the following command line arguments:

        - \c -h — prints a list of all registered tests.
        - \c -t \c \<name\> — runs only the test with the given name.
        - \c -f \c \<file\> — additionally writes test output to a log file.

        When executed without arguments, all registered tests are run.
        The exit code equals the number of errors, so a successful run
        returns 0.

        For more control over reporter setup or test execution, a custom
        %main() function can be written instead of including TestMain.h.

        @code
        #include <Pt/Unit/Application.h>
        #include <Pt/Unit/Reporter.h>

        int main()
        {
            Pt::Unit::Application app;
            Pt::Unit::BriefReporter reporter;
            app.attachReporter(reporter);
            app.run();
            return app.errors() > 0 ? 1 : 0;
        }
        @endcode

        @ingroup Pt-Unit
    */
    class PT_UNIT_API Application : private Test
    {
        public:
            /** @brief Default Constructor
            */
            Application();

            /** @brief Destructor
            */
            virtual ~Application();

            /** @brief Returns the instance.
            */
            static Application& instance();

            /** @brief Find a test by name

                Returns a pointer to the found test or 0 if not found.
            */
            Test* findTest(const std::string& testname);

            /** @brief Add reporter for test events

                Adds the reporter \a r to report test events.
            */
            void attachReporter(Reporter& r);

            /** @brief Add reporter for test events

                Adds the reporter \a r to report test events of the test
                name \a testname.
            */
            void attachReporter(Reporter& r, const std::string& testname);

            /** @brief Run test by name

                This method will run a previously registered test. Use the
                RegisterTest<T> template to register a test to the application.

                @param testName name of the test to be run
            */
            void run(const std::string& testName);

            /** @brief Run all tests

                This method will run all tests that have been registered
                previously. Use the RegisterTest<T> template to register
                a test to the application.
            */
            virtual void run();

            //! @brief Returns the number of errors which occured during a run
            unsigned errors() const
            { return _errors; }

            /** @brief Register a test

                Registers the test \a test to the application. The application
                will not own the test and the caller has to make sure it exists
                as long as the application object. Tests can be deregistered
                by calling %deregisterTest.
            */
            void registerTest(Test& test);

            void deregisterTest(Test& test);

            /** @brief Returns a list of all registered test
                TODO: find another way to query available tests
                @return Reference to the registered tests.
            */
            static std::list<Test*>& tests();

        private:
            static Application* _app;
            unsigned _errors;
    };

} // namespace Unit

} // namespace Pt

#endif
