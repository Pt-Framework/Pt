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
#ifndef PT_UNIT_TESTCASE_H
#define PT_UNIT_TESTCASE_H

#include <Pt/Unit/Test.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestFixture.h>


namespace Pt {

namespace Unit {

    /** @brief Single test with setup and teardown

        A %TestCase can be used for simple tests that require a initialization
        and deinitialization of resources. The implementor is supposed to
        implement the abstract method 'test' and the methods 'setUp' and
        'tearDown' for resource management. When the test is run, 'setUp'
        will be called first, then 'test' and finally 'tearDown'.

        @code
            class MyTest : public TestCase
            {
                public:
                    MyTest()
                    : TestCase("MyTest")
                    {}

                    virtual void setUp()
                    {
                        // init resource
                    }

                    virtual void tearDown()
                    {
                        // release resource
                    }

                    void test()
                    {
                        // test code using a resourc
                    }
            };
        @endcode

        Once the test is written it can be registered to an application by 
        using the RegisterTest class template.
    */
    class TestCase : public Test, public TestFixture
    {
        public:
            /** @brief Construct by name

                Constructs a %TestCase with the passed name.

                @param name Name of the test
            */
            TestCase(const std::string& name)
            : Test(name)
            { }

            /** @brief Runs the test
                When the test is run, 'setUp' will be called first, then 
                'test' and finally 'tearDown'. Signals inherited from
                Unit::Test are sent appropriatly.
            */
            virtual void run()
            {
                bool isUp = false;

                Test::started.send<const Test&>( *this );
                try
                {
                    this->setUp();
                    isUp = true;
                    this->test();
                    this->tearDown();
                    Test::success.send<const Test&>( *this );
                    return;
                }
                catch(const Assertion& assertion)
                {
                    Test::assertion.send<const Test&>( *this, assertion );
                }
                catch(const std::exception& ex)
                {
                    Test::exception.send<const Test&>( *this, ex );
                }
                catch(...)
                {
                    Test::error.send<const Test&>( *this );
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
                    Test::assertion.send<const Test&>( *this, assertion);
                }
                catch(const std::exception& ex)
                {
                    Test::exception.send<const Test&>( *this, ex);
                }
                catch(...)
                {
                    Test::error.send<const Test&>( *this );
                }

                Test::finished.send<const Test&>( *this );
            }

        protected:
            /** @brief Performs the actual test
                The implementor is supposed to override this method, which
                is called between 'setUp' and 'tearDown'. Assertions may be 
                thrown to indicate failed test assertions.
            */
            virtual void test() = 0;
    };

} // namespace Unit

} // namespace Pt

#endif // for header

