/***************************************************************************
 *   Copyright (C) 2005-2008 by Dr. Marc Boris Duerner                     *
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

#include <Pt/Unit/Api.h>
#include <Pt/Unit/Reporter.h>
#include <Pt/Unit/Test.h>
#include <sstream>

namespace Pt {

namespace Unit {

    /** @brief Run registered tests

        The application class serves as an environment for a number of tests
        to be run. An application object is usually created in the main loop
        of a program and the return value of Unit::Application::run returned.
        A reporter can be set for the application to process test events.
        Reporters can be made to print information to the console or write
        XML logs. A typical example may look like this:

        @code
            int main()
            {
                Pt::Unit::Reporter reporter;
                Pt::Unit::Application app;
                app.setReporter(reporter);
                return app.run();
            }
        @endcode

        The TestMain.h include already defines a main loop with an application
        for the common use case.
    */
    class PT_UNIT_API Application : public Test
    {
        template <typename TestT>
        friend struct RegisterTest;

        public:
            /** @brief Default Constructor
            */
            Application();

            /** @brief Destructor
            */
            virtual ~Application();

            static Application* instance();

            /** @brief Register a test

                Registers a test to the application. The application will
                not own the test and the caller has to make sure it exists
                as long as the application object. This method is called
                by the RegisterTest class template and does not need to
                be called directly.

                @param test Test to register
            */
            static void registerTest(Test& test);

            /** @brief Run all tests

                This method will run all tests that have been regietered
                previously. Use the RegisterTest<T> template to register
                a test to the application. The function will return the
                number of failed tests, so that its return value can directly
                be used to return from the main function.

                @return Number of failed tests.
            */
            void run();

            /** @brief Run test by name

                This method will a previously registered tests. Use the
                RegisterTest<T> template to register a test to the application.
                The function will return the number of failed tests, so that
                its return value can directly be used to return from the main
                function.

                @param testName name of the test to be run
                @return Number of failed tests.
            */
            void run(const std::string& testName);

            virtual void run(const SerializationInfo* si, std::size_t argCount);

            /** @brief Returns a list of all registered test

                @return Reference to the registered tests.
            */
            static std::list<Test*>& tests();

            //! @brief Returns the number of errors which occured during a run
            unsigned errors()
            { return _errors; }

        private:
            static Application* _app;

            /** @brief Number of errors that occured during a run
            */
            unsigned _errors;
    };

} // namespace Unit

} // namespace Pt

#endif
