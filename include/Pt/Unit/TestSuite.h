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
#ifndef PT_UNIT_TESTSUITE_H
#define PT_UNIT_TESTSUITE_H

#include <Pt/Unit/Test.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestFixture.h>
#include <Pt/Unit/TestProtocol.h>

namespace Pt {

namespace Unit {

    /** @brief Protocol and data driven testing

        The TestSuite is used to implement protocol and data driven tests.
        It inherits its ability to register methods and properties from
        %Reflectable. The implementor is supposed to write and register the 
        required test methods on construction.

        @code
            class MyTest : public TestSuite
            {
                public:
                    MyTest()
                    : TestSuite("MyTest")
                    {
                        this->registerMethod("test1", *this, &MyTest::test1);
                    }

                    void test1();
            };
        @endcode

        Once the test is written it can be registered to an application by 
        using the RegisterTest class template.

        The default protocol will run each registered test method when the 
        test is run. Before each test method setUp is called and tearDown 
        after each test. The TestProtocol can be replaced with a customised 
        one and reflection can be used to call any method multiple times with 
        the required data.
    */
    class TestSuite : public Test, public TestFixture
    {
        public:
            /** @brief Construct by name and protocol

                Constructs a %TestCase with the passed name and optionally
                a custom protocol. The protocol is not owned by the TestSuite,
                but can be owned by the derived class.

                @param name Name of the test
                @param protocol Protocol for the test.
            */
            TestSuite(const std::string& name, TestProtocol& protocol = TestSuite::defaultProtocol)
            : Test(name)
            , _protocol(&protocol)
            { }

            /** @brief Runs the test suite

                The TestProtocol assosiated with the test will be executed.
                The default protocol will simply call all registered tests.
            */
            virtual void run()
            {
                _protocol->run(*this);
            }

            /** @brief Runs a registered test

                A test method will be called by name and the given arguments
                are passe to it just like when the reflection API is used.
                The method 'setUp' will be called before, and the method
                tearDown after the test. Signals inherited from Unit::Test
                are sent appropriatly.

                @param name Name of the method to be run
                @param args Arguments to invoke the method
            */
            void runTest( const std::string& name, const Args& args = Args() )
            {
                // TODO: use a sentry object
                bool isUp = false;

                Test::started.send<const Test&>( *this );
                try
                {
                    this->setUp();
                    isUp = true;
                    Reflectable::call(name, args);
                    this->tearDown();
                    Test::success.send<const Test&>( *this );
                    return;
                }
                catch(const Assertion& assertion)
                {
                    Test::assertion.send<const Test&>(*this, assertion);
                }
                catch(const std::exception& ex)
                {
                    Test::exception.send<const Test&>(*this, ex);
                }
                catch(...)
                {
                    Test::error.send<const Test&>(*this);
                }

                try
                {
                    if(isUp)
                    {
                        this->tearDown();
                    }
                }
                catch(const Assertion& assertion)
                {
                    Test::assertion.send<const Test&>(*this, assertion);
                }
                catch(const std::exception& ex)
                {
                    Test::exception.send<const Test&>(*this, ex);
                }
                catch(...)
                {
                    Test::error.send<const Test&>(*this);
                }

                Test::finished.send<const Test&>( *this );
            }

        protected:
            /** @brief The assosiated test protocol
            */
            TestProtocol* _protocol;

        public:
            static TestProtocol defaultProtocol;
    };

    TestProtocol TestSuite::defaultProtocol;


    inline void TestProtocol::run(TestSuite& test)
    {
        const MethodMap& methods = test.methods();
        MethodMap::const_iterator it;
        for(it = methods.begin(); it != methods.end(); ++it)
        {
            test.runTest( it->first, Args() );
        }
    }

} // namespace Unit

} // namespace Pt

#endif // for header

