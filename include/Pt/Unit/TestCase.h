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

#include <Pt/Unit/Api.h>
#include <Pt/Unit/Test.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestContext.h>

#include <string>


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
    class TestCase : public Test
    {
        public:
            class ConText : public TestContext
            {
                public:
                    ConText(TestCase& test)
                    : TestContext(test)
                    , _test(test)
                    , _setUp(false)
                    { }

                    ~ConText()
                    {
                        try
                        {
                            if( _setUp )
                                _test.tearDown();
                        }
                        catch(...)
                        {}
                    }

                    const std::string& testName() const
                    { return _test.name(); }

                protected:
                    void _run()
                    {
                        _test.setUp();
                        _setUp = true;
                        _test.test();
                    }

                private:
                    TestCase& _test;
                    bool _setUp;
            };

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
                ConText ctx(*this);
                ctx.run();
            }

            /** \brief Set up conText before running a test.

                This function is called before each registered tester function
                is invoked. It is meant to initialize any required resources.
            */
            virtual void setUp()
            {}

            /** \brief Clean up after the test run.

                This function is called after each registered tester function
                is invoked. It is meant to remove any resources previously
                initialized in TestCase::setUp.
            */
            virtual void tearDown()
            {}

        protected:
            /** @brief Performs the actual test

                The implementor is supposed to override this method, which
                is called between 'setUp' and 'tearDown'. Assertions may be
                thrown to indicate failed test assertions.
            */
            virtual void test()
            { }
    };

} // namespace Unit

} // namespace Pt

#endif // for header

